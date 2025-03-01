#include <game_object.h>
#include <module/animator.h>
#include <resource_manager.h>
#include <scene.h>

#include <functional>
#include <log.hpp>
#include <media_utils.hpp>
#include <module/collision_component.hpp>
#include <module/collision_manager.hpp>
#include <module/image.hpp>
#include <module/rigidbody_manager.hpp>

namespace gameaf {

GameObject::GameObject() = default;
GameObject::GameObject(const std::string& name) : m_name(name) {}
GameObject::GameObject(int z_order, const std::string& name) : m_name(name), m_zOrder(z_order) {}
GameObject::~GameObject()
{
    CollisionManager::getInstance().deleteCollisionComponent(m_collision_component);
    RigidbodyManager::getInstance().deleteRigidbody2D(m_rigidbody2D);
}

GameObject::GameObject(const GameObject& obj)
{
    m_enterCallback = obj.m_enterCallback;
    m_updateCallback = obj.m_updateCallback;
    // 资源复制
    m_name = obj.m_name;
    m_zOrder = obj.m_zOrder;
    m_myScene = obj.m_myScene;
    m_position = obj.m_position;
    m_img_size = obj.m_img_size;
    m_anchor_mode = obj.m_anchor_mode;
    m_anchor_position = obj.m_anchor_position;
    m_image = obj.m_image;
    if (obj.m_animator != nullptr) m_animator = std::make_unique<Animator>(*(obj.m_animator));
    // 子对象全部复制一遍
    if (obj.m_child_gameObjects) {
        m_child_gameObjects =
            std::make_unique<std::vector<GameObjectPtr>>(obj.m_child_gameObjects->size());
        for (auto& child_object : *m_child_gameObjects) {
            m_child_gameObjects->emplace_back(child_object->clone());
        }
    }
    m_parent = obj.m_parent;
    m_collision_component =
        CollisionManager::getInstance().copyCollisionComponent(obj.m_collision_component);
    m_rigidbody2D = RigidbodyManager::getInstance().copyRigidbody2D(obj.m_rigidbody2D);
    gameaf::log("[debug] 对象{}复制成功......", m_name);
}

void GameObject::swap(GameObject& mv_obj)
{
    m_enterCallback = mv_obj.m_enterCallback;
    m_updateCallback = mv_obj.m_updateCallback;
    // 资源转移
    mv_obj.m_name.swap(m_name);
    m_zOrder = mv_obj.m_zOrder;
    m_myScene = mv_obj.m_myScene;
    m_position = mv_obj.m_position;
    m_img_size = mv_obj.m_img_size;
    m_anchor_mode = mv_obj.m_anchor_mode;
    m_anchor_position = mv_obj.m_anchor_position;
    m_image = mv_obj.m_image;
    m_animator = std::move(mv_obj.m_animator);
    // 子对象全部直接转移
    if (mv_obj.m_child_gameObjects) {
        m_child_gameObjects = std::move(mv_obj.m_child_gameObjects);
    }
    m_parent = mv_obj.m_parent;
    m_collision_component = mv_obj.m_collision_component;
    mv_obj.m_collision_component = nullptr;
    m_rigidbody2D = mv_obj.m_rigidbody2D;
    mv_obj.m_rigidbody2D = nullptr;
}

GameObject::GameObject(GameObject&& mv_obj)
{
    swap(mv_obj);
    gameaf::log("[debug] 对象{}转移成功......", m_name);
}
GameObject& GameObject::operator=(const GameObject& obj)
{
    GameObject copy{obj};
    swap(copy);
    return *this;
}
GameObject& GameObject::operator=(GameObject&& obj)
{
    swap(obj);
    gameaf::log("[debug] 对象{}转移成功......", m_name);
    return *this;
}

GameObject::GameObjectPtr GameObject::clone()
{
    auto clone_ptr = std::make_shared<GameObject>(*this);
    if (auto parent_ptr = clone_ptr->m_parent.lock()) {
        parent_ptr->addChildObject(clone_ptr);
    }
    return clone_ptr;
}

void GameObject::detachChildObject(GameObject* detach_child)
{
    if (m_child_gameObjects) {
        for (auto it = m_child_gameObjects->begin(); it != m_child_gameObjects->end();) {
            if (it->get() == detach_child) {
                m_child_gameObjects->erase(it);
                break;
            }
        }
    }
}

void GameObject::addChildObject(GameObjectPtr child)
{
    auto parent_ptr = child->m_parent.lock();
    if (parent_ptr && parent_ptr.get() != this) {
        parent_ptr->detachChildObject(child.get());
    }
    child->m_parent = shared_from_this();
    if (m_child_gameObjects == nullptr) {
        m_child_gameObjects = std::make_unique<std::vector<GameObjectPtr>>();
    }
    m_child_gameObjects->emplace_back(std::move(child));
}

void GameObject::addChildObjects(const std::vector<GameObjectPtr>& children)
{
    for (auto& child : children) {
        addChildObject(child);
    }
}

GameObject::GameObjectPtr GameObject::getChildObject(size_t index)
{
    if (m_child_gameObjects == nullptr || m_child_gameObjects->size() <= index) return nullptr;
    return m_child_gameObjects->at(index);
}

std::vector<GameObject::GameObjectPtr> GameObject::findChildObjects(const std::string& child_id)
{
    return {};
}

void GameObject::onUpdate(float delta)
{
    // 特殊属性处理更新
    if (m_animator) m_animator->onUpdate(delta);
    if (m_child_gameObjects) {
        for (auto& game_object : *m_child_gameObjects) {
            game_object->onUpdate();
            game_object->onUpdate(delta);
        }
    }
}

/// @brief 辅助计算渲染目标矩形
inline Rect buildRender(const Vector2& pos, const Vector2& size, GameObject::AnchorMode mode,
                        const Vector2& anchor_pos = {0.0f, 0.0f})
{
    Rect dst;
    dst.w = static_cast<int>(size.X);
    dst.h = static_cast<int>(size.Y);

    switch (mode) {
        case GameObject::AnchorMode::Centered:
            dst.x = pos.X - size.X / 2;
            dst.y = pos.Y - size.Y / 2;
            break;
        case GameObject::AnchorMode::BottomCentered:
            dst.x = pos.X - size.X / 2;
            dst.y = pos.Y - size.Y;
            break;
        case GameObject::AnchorMode::TopCentered:
            dst.x = pos.X - size.X / 2;
            dst.y = pos.Y;
            break;
        case GameObject::AnchorMode::Customized:
            // 注意，如果anchor_pos不符合0.0~1.0的范围,则视为0进行处理
            dst.x = static_cast<int>(pos.X - size.X * anchor_pos.X);
            dst.y = static_cast<int>(pos.Y - size.Y * anchor_pos.Y);
            break;
        default:
            break;
    }
    return dst;
}

void GameObject::onRender(const Camera& camera)
{
    // 特殊属性处理渲染
    if (m_image) {
        putImageEx(camera, *m_image,
                   buildRender(m_position, m_img_size, m_anchor_mode, m_anchor_position));
    }

    if (m_animator) {
        m_animator->onRender(camera, buildRender(m_position, m_animator->currentAnimationSize(),
                                                 m_anchor_mode, m_anchor_position));
    }

    if (m_child_gameObjects) {
        for (auto& game_object : *m_child_gameObjects) {
            game_object->onRender(camera);
        }
    }

    // debug render
    if (m_collision_component) m_collision_component->onDebugRender(camera);
}

void GameObject::setZOrder(int z_order)
{
    if (z_order != m_zOrder) {
        m_zOrder = z_order;
        // 如果存在上层场景管理, 通知场景需要更新渲染顺序了, 场景会在下次渲染的时候更新排序,
        // 按照设置的渲染顺序进行渲染
        if (auto scene_ptr = m_myScene.lock()) scene_ptr->notifyRenderLayerUpdate();
    }
}
}  // namespace gameaf