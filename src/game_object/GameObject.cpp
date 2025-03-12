#include "GameObject.h"

#include <game_object/component/Animator.h>
#include <game_object/component/Image.h>
#include <game_object/component/Rigidbody2D.h>
#include <game_object/component/Text.h>
#include <resource/ResourceManager.h>
#include <scene/Scene.h>

#include <common/Log.hpp>
#include <common/MediaUtils.hpp>
#include <functional>
#include <game_object/component/CollisionManager.hpp>

namespace gameaf {

GameObject::GameObject() = default;
GameObject::GameObject(const std::string& name) : m_name(name) {}
GameObject::GameObject(int z_order, const std::string& name) : m_name(name), m_zOrder(z_order) {}
GameObject::~GameObject()
{
    if (m_collisions) {
        for (auto collion : *m_collisions) {
            CollisionManager::GetInstance().DeleteCollisionComponent(collion);
        }
    }
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
    if (obj.m_image != nullptr) {
        m_image = std::make_unique<Image>(*(obj.m_image));
        m_image->SetGameObject(this);
    }
    if (obj.m_animator != nullptr) {
        m_animator = std::make_unique<Animator>(*(obj.m_animator));
        m_animator->SetGameObject(this);
    }
    // 子对象全部复制一遍
    if (obj.m_child_gameObjects) {
        m_child_gameObjects =
            std::make_unique<std::vector<GameObjectPtr>>(obj.m_child_gameObjects->size());
        for (auto& child_object : *obj.m_child_gameObjects) {
            m_child_gameObjects->emplace_back(child_object->Clone());
        }
    }
    m_parent = obj.m_parent;
    if (obj.m_collisions) {
        m_collisions = std::make_unique<std::vector<CollisionPtr>>(obj.m_collisions->size());
        for (auto& collision : *obj.m_collisions) {
            m_collisions->emplace_back(
                CollisionManager::GetInstance().CopyCollision(this, collision));
        }
    }
    if (obj.m_rigidbody2D != nullptr) {
        m_rigidbody2D = std::make_unique<Rigidbody2D>(*(obj.m_rigidbody2D));
        m_rigidbody2D->SetGameObject(this);
    }
    if (obj.m_text != nullptr) {
        m_text = std::make_unique<Text>(*(obj.m_text));
        m_text->SetGameObject(this);
    }
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
    m_image = std::move(mv_obj.m_image);
    m_image->SetGameObject(this);
    m_animator = std::move(mv_obj.m_animator);
    m_animator->SetGameObject(this);
    // 子对象全部直接转移
    if (mv_obj.m_child_gameObjects) {
        m_child_gameObjects = std::move(mv_obj.m_child_gameObjects);
    }
    m_parent = mv_obj.m_parent;
    m_collisions = std::move(mv_obj.m_collisions);
    if (m_collisions) {
        for (auto collision : *m_collisions) {
            collision->SetGameObject(this);
        }
    }
    m_rigidbody2D = std::move(mv_obj.m_rigidbody2D);
    m_rigidbody2D->SetGameObject(this);
    m_text = std::move(mv_obj.m_text);
    m_text->SetGameObject(this);
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
    std::vector<GameObject::GameObjectPtr> findObjects;
    if (m_child_gameObjects) {
        for (auto& child : *m_child_gameObjects) {
            if (child->GetName() == child_id) findObjects.emplace_back(child);
        }
    }
    return findObjects;
}

void GameObject::DelChildObjects(const std::string& child_id)
{
    if (m_child_gameObjects) {
        auto it = m_child_gameObjects->begin();
        while (it != m_child_gameObjects->end()) {
            if ((*it)->GetName() == child_id) {
                it = m_child_gameObjects->erase(it);
            } else {
                ++it;
            }
        }
    }
}

void GameObject::OnFixUpdate(float alpha)
{
    // 特殊属性处理更新
    if (m_rigidbody2D) m_rigidbody2D->OnFixedUpdate(alpha);
    if (m_child_gameObjects) {
        for (auto& game_object : *m_child_gameObjects) {
            game_object->OnFixUpdate();
            game_object->OnFixUpdate(alpha);
        }
    }
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

    if (m_text) {
        m_text->OnRender(camera);
    }

    if (m_child_gameObjects) {
        for (auto& game_object : *m_child_gameObjects) {
            game_object->OnRender(camera);
            game_object->OnDraw(camera);
        }
    }
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

void GameObject::Flip()
{
    if (m_image) {
        m_image->Flip();
    }

    if (m_animator) {
        m_animator->Flip();
    }

    if (m_child_gameObjects) {
        for (auto& game_object : *m_child_gameObjects) {
            game_object->Flip();
        }
    }

    // debug render
    if (m_collisions) {
        for (auto collision : *m_collisions) {
            collision->Flip();
        }
    }
}

template <typename T, typename... Args>
std::enable_if_t<std::is_base_of_v<Component, T>, T*> GameObject::CreateComponent(Args&&... args)
{
    if constexpr (std::is_same_v<T, Image>) {
        if (m_image) return nullptr;
        // 检查类型, 使用tuple严格匹配参数类型
        // std::decay_t 方便将const、&去除
        // if constexpr (std::is_same_v<std::tuple<std::decay_t<Args>...>, std::tuple<std::string>>) {
        if constexpr (sizeof...(Args) == 1) {
            // img_id
            m_image = std::make_unique<Image>(
                this, ResourceManager::GetInstance().GetTImage(std::forward<Args>(args)...));
        }
        // else if constexpr (std::is_same_v<std::tuple<std::decay_t<Args>...>, std::tuple<std::string, Vector2>>) {
        // else {
        //     auto argsTuple = std::forward_as_tuple(std::forward<Args>(args)...);  // 保留左右值属性
        //     m_image = std::make_unique<Image>(
        //         this, ResourceManager::GetInstance().GetTImage(std::get<0>(argsTuple)),
        //         std::get<1>(argsTuple));
        // }
        // else
        // {
        //     static_assert(false, "Image组件参数: (imgId[, offset])");
        // }
        return m_image.get();
    } else if constexpr (std::is_same_v<T, Animator>) {
        // 没有参数传递
        static_assert(sizeof...(Args) == 0, "Animator组件未知参数传入");
        if (m_animator) return nullptr;
        m_animator = std::make_unique<Animator>(this);
        return m_animator.get();
    } else if constexpr (std::is_base_of_v<Collision, T>) {
        static_assert(sizeof...(Args) == 0 ||
                          std::is_same_v<std::tuple<std::decay_t<Args>...>, std::tuple<Vector2>>,
                      "Collision组件参数: ([offset])");
        if (m_collisions == nullptr) {
            m_collisions = std::make_unique<std::vector<CollisionPtr>>();
        }

        T* collision = nullptr;
        collision =
            CollisionManager::GetInstance().CreateCollision<T>(this, std::forward<Args>(args)...);

        if constexpr (std::is_same_v<CollisionBox, T>) {
            if (m_image) {
                collision->SetSize(m_image->GetSize());
            } else if (m_animator) {
                collision->SetSize(m_animator->GetInitialAnimation().CurrentFrameSize());
            }
        }
        m_collisions->emplace_back(collision);
        return collision;
    } else if constexpr (std::is_same_v<T, Rigidbody2D>) {
        static_assert(sizeof...(Args) == 0, "Rigidbody2D组件未知参数传入");
        if (m_rigidbody2D) return nullptr;
        m_rigidbody2D = std::make_unique<Rigidbody2D>(this);
        return m_rigidbody2D.get();
    } else if constexpr (std::is_same_v<T, Text>) {
        // static_assert(std::is_same_v<std::tuple<std::decay_t<Args>...>, std::tuple<std::wstring>> ||
        //                   std::is_same_v<std::tuple<std::decay_t<Args>...>,
        //                                  std::tuple<std::wstring, Vector2>>,
        //               "Text组件参数: (fontName-wstr[, offset])");
        if (m_text) return nullptr;
        m_text = std::make_unique<Text>(this, std::forward<Args>(args)...);
        return m_text.get();
    } else {
        static_assert(false, "非法组件创建");
    }

    return nullptr;
}

// 获取组件指针
template <typename T>
std::enable_if_t<std::is_base_of_v<Component, T>, T*> GameObject::GetComponent()
{
    return nullptr;
}
template <>
Image* GameObject::GetComponent<Image>()
{
    return m_image.get();
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
    if (m_collisions == nullptr) return nullptr;
    for (auto collision : *m_collisions) {
        if (collision->Type() == CollisionType::Box) {
            return static_cast<CollisionBox*>(collision);
        }
    }
    return nullptr;
}

template <>
Rigidbody2D* GameObject::GetComponent<Rigidbody2D>()
{
    if (m_rigidbody2D == nullptr) return nullptr;
    return m_rigidbody2D.get();
}

template <>
Text* GameObject::GetComponent<Text>()
{
    if (m_text == nullptr) return nullptr;
    return m_text.get();
}

// 显示实例化模板函数
template Image* GameObject::CreateComponent<Image>(const std::string&);
template Image* GameObject::CreateComponent<Image>(std::string&&);

template Animator* GameObject::CreateComponent<Animator>();

template CollisionBox* GameObject::CreateComponent<CollisionBox>();
template CollisionBox* GameObject::CreateComponent<CollisionBox>(Vector2&& offset);
template CollisionBox* GameObject::CreateComponent<CollisionBox>(const Vector2& offset);

template CollisionRaycaster* GameObject::CreateComponent<CollisionRaycaster>();
template CollisionRaycaster* GameObject::CreateComponent<CollisionRaycaster>(const Vector2& offset);
template CollisionRaycaster* GameObject::CreateComponent<CollisionRaycaster>(Vector2&& offset);

template Rigidbody2D* GameObject::CreateComponent<Rigidbody2D>();

template Text* GameObject::CreateComponent<Text>(const std::wstring&);
template Text* GameObject::CreateComponent<Text>(std::wstring&&);

template Image* GameObject::GetComponent<Image>();
template Animator* GameObject::GetComponent<Animator>();
template CollisionBox* GameObject::GetComponent<CollisionBox>();
template Rigidbody2D* GameObject::GetComponent<Rigidbody2D>();
template Text* GameObject::GetComponent<Text>();

}  // namespace gameaf