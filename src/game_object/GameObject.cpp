#include "GameObject.h"

#include <game_object/component/Animator.h>
#include <game_object/component/Collision.h>
#include <game_object/component/Image.h>
#include <resource/ResourceManager.h>
#include <scene/Scene.h>

#include <common/Log.hpp>
#include <common/MediaUtils.hpp>
#include <functional>
#include <game_object/component/CollisionManager.hpp>
#include <game_object/component/RigidbodyManager.hpp>

namespace gameaf {

GameObject::GameObject() = default;
GameObject::GameObject(const std::string& name) : m_name(name) {}
GameObject::GameObject(int z_order, const std::string& name) : m_name(name), m_zOrder(z_order) {}
GameObject::~GameObject()
{
    CollisionManager::GetInstance().DeleteCollisionComponent(m_collision);
    RigidbodyManager::GetInstance().DeleteRigidbody2D(m_rigidbody2D);
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
    m_image = obj.m_image;
    if (obj.m_animator != nullptr) m_animator = std::make_unique<Animator>(*(obj.m_animator));
    // 子对象全部复制一遍
    if (obj.m_child_gameObjects) {
        m_child_gameObjects =
            std::make_unique<std::vector<GameObjectPtr>>(obj.m_child_gameObjects->size());
        for (auto& child_object : *m_child_gameObjects) {
            m_child_gameObjects->emplace_back(child_object->Clone());
        }
    }
    m_parent = obj.m_parent;
    m_collision = CollisionManager::GetInstance().CopyCollision(obj.m_collision);
    m_rigidbody2D = RigidbodyManager::GetInstance().CopyRigidbody2D(obj.m_rigidbody2D);
    gameaf::log("[debug] 对象{}复制成功......", m_name);
}

void GameObject::Swap(GameObject& mv_obj)
{
    m_enterCallback = mv_obj.m_enterCallback;
    m_updateCallback = mv_obj.m_updateCallback;
    // 资源转移
    mv_obj.m_name.swap(m_name);
    m_zOrder = mv_obj.m_zOrder;
    m_myScene = mv_obj.m_myScene;
    m_position = mv_obj.m_position;
    m_image = mv_obj.m_image;
    m_animator = std::move(mv_obj.m_animator);
    // 子对象全部直接转移
    if (mv_obj.m_child_gameObjects) {
        m_child_gameObjects = std::move(mv_obj.m_child_gameObjects);
    }
    m_parent = mv_obj.m_parent;
    m_collision = mv_obj.m_collision;
    mv_obj.m_collision = nullptr;
    m_rigidbody2D = mv_obj.m_rigidbody2D;
    mv_obj.m_rigidbody2D = nullptr;
}

GameObject::GameObject(GameObject&& mv_obj)
{
    Swap(mv_obj);
    gameaf::log("[debug] 对象{}转移成功......", m_name);
}

GameObject& GameObject::operator=(const GameObject& obj)
{
    GameObject copy{obj};
    Swap(copy);
    return *this;
}

GameObject& GameObject::operator=(GameObject&& obj)
{
    Swap(obj);
    gameaf::log("[debug] 对象{}转移成功......", m_name);
    return *this;
}

GameObject::GameObjectPtr GameObject::Clone()
{
    auto clone_ptr = std::make_shared<GameObject>(*this);
    if (auto parent_ptr = clone_ptr->m_parent.lock()) {
        parent_ptr->AddChildObject(clone_ptr);
    }
    return clone_ptr;
}

void GameObject::DetachChildObject(GameObject* detach_child)
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

void GameObject::AddChildObject(GameObjectPtr child)
{
    auto parent_ptr = child->m_parent.lock();
    if (parent_ptr && parent_ptr.get() != this) {
        parent_ptr->DetachChildObject(child.get());
    }
    child->m_parent = shared_from_this();
    if (m_child_gameObjects == nullptr) {
        m_child_gameObjects = std::make_unique<std::vector<GameObjectPtr>>();
    }
    m_child_gameObjects->emplace_back(std::move(child));
}

void GameObject::AddChildObjects(const std::vector<GameObjectPtr>& children)
{
    for (auto& child : children) {
        AddChildObject(child);
    }
}

GameObject::GameObjectPtr GameObject::GetChildObject(size_t index)
{
    if (m_child_gameObjects == nullptr || m_child_gameObjects->size() <= index) return nullptr;
    return m_child_gameObjects->at(index);
}

std::vector<GameObject::GameObjectPtr> GameObject::FindChildObjects(const std::string& child_id)
{
    return {};
}

void GameObject::OnUpdate(float delta)
{
    // 特殊属性处理更新
    if (m_animator) m_animator->OnUpdate(delta);
    if (m_child_gameObjects) {
        for (auto& game_object : *m_child_gameObjects) {
            game_object->OnUpdate();
            game_object->OnUpdate(delta);
        }
    }
}

void GameObject::OnRender(const Camera& camera)
{
    // 特殊属性处理渲染
    if (m_image) {
        m_image->OnRender(camera);
    }

    if (m_animator) {
        m_animator->OnRender(camera);
    }

    if (m_child_gameObjects) {
        for (auto& game_object : *m_child_gameObjects) {
            game_object->OnRender(camera);
        }
    }

    // debug render
    if (m_collision) m_collision->OnDebugRender(camera);
}

void GameObject::SetZOrder(int z_order)
{
    if (z_order != m_zOrder) {
        m_zOrder = z_order;
        // 如果存在上层场景管理, 通知场景需要更新渲染顺序了, 场景会在下次渲染的时候更新排序,
        // 按照设置的渲染顺序进行渲染
        if (auto scene_ptr = m_myScene.lock()) scene_ptr->NotifyRenderLayerUpdate();
    }
}

template <typename T, typename... Args>
std::enable_if_t<std::is_same_v<T, Image> || std::is_same_v<T, Animator> ||
                     std::is_same_v<T, CollisionBox> || std::is_same_v<T, Rigidbody2D>,
                 T*>
GameObject::CreateComponent(Args&&... args)
{
    if constexpr (std::is_same_v<T, Image>) {
        // 检查类型, 使用tuple严格匹配参数类型
        // std::decay_t 方便将const、&去除
        static_assert(std::is_same_v<std::tuple<std::decay_t<Args>...>, std::tuple<std::string>>);
        if (m_image) return nullptr;
        // img_id
        m_image = ResourceManager::GetInstance().GetImage(std::forward<Args>(args)...);
        m_image->SetGameObject(this);
        return m_image;
    } else if constexpr (std::is_same_v<T, Animator>) {
        // 没有参数传递
        static_assert(sizeof...(Args) == 0);
        if (m_animator) return nullptr;
        m_animator = std::make_unique<Animator>(this);
        return m_animator.get();
    } else if constexpr (std::is_same_v<T, CollisionBox>) {
        static_assert(sizeof...(Args) == 0 ||
                      std::is_same_v<std::tuple<std::decay_t<Args>...>, std::tuple<Vector2>>);
        if (m_collision) return nullptr;

        CollisionBox* collision_box = nullptr;
        if constexpr (sizeof...(Args) == 0) {
            collision_box = CollisionManager::GetInstance().CreateCollisionBox(this, Vector2{});
        } else {
            collision_box = CollisionManager::GetInstance().CreateCollisionBox(
                this, std::forward<Args>(args)...);
        }
        if (m_image) {
            collision_box->SetSize(m_image->GetSize());
        } else if (m_animator) {
            collision_box->SetSize(m_animator->GetInitialAnimation().CurrentFrameSize());
        }
        m_collision = collision_box;
        return collision_box;
    } else if constexpr (std::is_same_v<T, Rigidbody2D>) {
        static_assert(sizeof...(Args) == 0);
        if (m_rigidbody2D) return nullptr;
        m_rigidbody2D = RigidbodyManager::GetInstance().CreateRigidbody2D(this);
        return m_rigidbody2D;
    }

    return nullptr;
}

// 获取组件指针
template <typename T>
T* GameObject::GetComponent()
{
    return nullptr;
}
template <>
Image* GameObject::GetComponent<Image>()
{
    return m_image;
}
template <>
Animator* GameObject::GetComponent<Animator>()
{
    if (m_animator == nullptr) return nullptr;
    return m_animator.get();
}
template <>
CollisionBox* GameObject::GetComponent<CollisionBox>()
{
    if (m_collision == nullptr || m_collision->Type() != CollisionType::Box) return nullptr;
    return static_cast<CollisionBox*>(m_collision);
}

template <>
Rigidbody2D* GameObject::GetComponent<Rigidbody2D>()
{
    return m_rigidbody2D;
}

// 显示实例化模板函数
template Image* GameObject::CreateComponent<Image, const std::string&>(const std::string&);
template Animator* GameObject::CreateComponent<Animator>();
template CollisionBox* GameObject::CreateComponent<CollisionBox>();
template Rigidbody2D* GameObject::CreateComponent<Rigidbody2D>();

template Image* GameObject::GetComponent<Image>();
template Animator* GameObject::GetComponent<Animator>();
template CollisionBox* GameObject::GetComponent<CollisionBox>();
template Rigidbody2D* GameObject::GetComponent<Rigidbody2D>();

}  // namespace gameaf