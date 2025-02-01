#include "../include/game_object.h"

#include <functional>
#include <unordered_map>

#include "../common/media_utils.hpp"
#include "../game_object/module/animator.h"
#include "../game_object/module/image.hpp"
#include "../include/log.hpp"
#include "../include/resource_manager.h"
#include "../include/scene.h"

namespace gameaf {

GameObject::GameObject() = default;
GameObject::GameObject(const std::string& name) : m_name(name) {}
GameObject::GameObject(int z_order, const std::string& name) : m_name(name), m_zOrder(z_order) {}
GameObject::~GameObject() = default;

GameObject::GameObject(const GameObject& obj)
{
    // 资源复制
    m_name = obj.m_name;
    m_zOrder = obj.m_zOrder;
    m_myScene = obj.m_myScene;
    m_position = obj.m_position;
    m_size = obj.m_size;
    m_anchor_mode = obj.m_anchor_mode;
    m_anchor_position = obj.m_anchor_position;
    m_image = obj.m_image;
    m_animator = std::make_unique<Animator>(*(obj.m_animator));
    // 子对象全部复制一遍
    if (obj.m_child_gameObjects) {
        m_child_gameObjects =
            std::make_unique<std::vector<GameObjectPtr>>(obj.m_child_gameObjects->size());
        for (auto& child_object : *m_child_gameObjects) {
            m_child_gameObjects->emplace_back(child_object->clone());
        }
    }
    m_parent = obj.m_parent;
    gameaf::log("[debug] 对象{}复制成功......", m_name);
}

void GameObject::swap(GameObject& mv_obj)
{
    // 资源转移
    mv_obj.m_name.swap(m_name);
    m_zOrder = mv_obj.m_zOrder;
    m_myScene = mv_obj.m_myScene;
    m_position = mv_obj.m_position;
    m_size = mv_obj.m_size;
    m_anchor_mode = mv_obj.m_anchor_mode;
    m_anchor_position = mv_obj.m_anchor_position;
    m_image = mv_obj.m_image;
    m_animator = std::move(mv_obj.m_animator);
    // 子对象全部直接转移
    if (mv_obj.m_child_gameObjects) {
        m_child_gameObjects = std::move(mv_obj.m_child_gameObjects);
    }
    m_parent = mv_obj.m_parent;
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

bool GameObject::newImage(const std::string& img_id)
{
    ImagePtr new_img = ResourceManager::getInstance().getImage(img_id);
    if (new_img == nullptr) return false;
    m_image = new_img;
    return true;
}

void GameObject::adaptImageSize()
{
    if (m_image) {
        m_size = {m_image->getWidth() * 1.0f, m_image->getHeight() * 1.0f};
    }
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
            game_object->onUpdate(delta);
            game_object->onUpdate();
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
    static std::unordered_map<
        GameObject::AnchorMode,
        std::function<void(Rect&, const Vector2&, const Vector2&, const Vector2&)>>
        map_fuc = {
            {GameObject::AnchorMode::Centered,
             [](Rect& dst, const Vector2& pos, const Vector2& size, const Vector2& anchor_pos) {
                 dst.x = static_cast<int>(pos.X - size.X / 2);
                 dst.y = static_cast<int>(pos.Y - size.Y / 2);
             }},
            {GameObject::AnchorMode::BottomCentered,
             [](Rect& dst, const Vector2& pos, const Vector2& size, const Vector2& anchor_pos) {
                 dst.x = static_cast<int>(pos.X - size.X / 2);
                 dst.y = static_cast<int>(pos.Y - size.Y);
             }},
            {GameObject::AnchorMode::TopCentered,
             [](Rect& dst, const Vector2& pos, const Vector2& size, const Vector2& anchor_pos) {
                 dst.x = static_cast<int>(pos.X - size.X / 2);
                 dst.y = static_cast<int>(pos.Y);
             }},
            {GameObject::AnchorMode::Customized,
             [](Rect& dst, const Vector2& pos, const Vector2& size, const Vector2& anchor_pos) {
                 // 注意，如果anchor_pos不符合0.0~1.0的范围,则视为0进行处理
                 dst.x = static_cast<int>(pos.X - size.X * anchor_pos.X);
                 dst.y = static_cast<int>(pos.Y - size.Y * anchor_pos.Y);
             }},
        };

    map_fuc[mode](dst, pos, size, anchor_pos);
    return dst;
}

void GameObject::onRender(const Camera& camera)
{
    Rect build_rect = buildRender(m_position, m_size, m_anchor_mode, m_anchor_position);
    // 特殊属性处理渲染
    if (m_image) putImageEx(camera, *m_image, build_rect);
    if (m_animator) m_animator->onRender(camera, build_rect);
    if (m_child_gameObjects) {
        for (auto& game_object : *m_child_gameObjects) {
            game_object->onRender(camera);
        }
    }
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