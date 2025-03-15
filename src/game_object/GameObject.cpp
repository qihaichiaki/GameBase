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
    m_isAwake = obj.m_isAwake;
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
    if (obj.m_texts != nullptr) {
        m_texts = std::make_unique<std::vector<TextPtr>>();
        for (auto& text : *obj.m_texts) {
            auto copyText = std::make_unique<Text>(*(text));
            copyText->SetGameObject(this);
            m_texts->emplace_back(std::move(copyText));
        }
    }

    if (obj.m_cameras != nullptr) {
        m_cameras = std::make_unique<std::vector<Camera*>>(*obj.m_cameras);
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
    m_isAwake = mv_obj.m_isAwake;
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
    m_texts = std::move(mv_obj.m_texts);
    if (m_texts) {
        for (auto& text : *m_texts) {
            text->SetGameObject(this);
        }
    }

    m_cameras = std::move(mv_obj.m_cameras);
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
    auto parent_ptr = child->m_parent;
    if (parent_ptr && parent_ptr != this) {
        parent_ptr->DetachChildObject(child.get());
    }

    child->m_parent = this;
    if (!child->m_isAwake) {
        child->OnAwake();
        child->m_isAwake = true;
    }
    child->m_cameras = nullptr;
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
                if ((*it).get() == m_containsScreenPointChildObj) {
                    m_containsScreenPointChildObj = nullptr;
                }
                it = m_child_gameObjects->erase(it);
            } else {
                ++it;
            }
        }
    }
}

void GameObject::OnFixUpdate(float alpha)
{
    if (m_isActive) {
        // 特殊属性处理更新
        if (m_rigidbody2D) m_rigidbody2D->OnFixedUpdate(alpha);
        OnFixUpdate();
    }

    if (m_isChildrenActive && m_child_gameObjects) {
        for (auto& game_object : *m_child_gameObjects) {
            if (game_object->GetActive()) {
                game_object->OnFixUpdate();
                game_object->OnFixUpdate(alpha);
            }
        }
    }
}

void GameObject::OnUpdate(float delta)
{
    if (m_isActive) {
        // 特殊属性处理更新
        if (m_animator) m_animator->OnUpdate(delta);
        OnUpdate();
    }

    if (m_isChildrenActive && m_child_gameObjects) {
        for (auto& game_object : *m_child_gameObjects) {
            if (game_object->GetActive()) {
                game_object->OnUpdate();
                game_object->OnUpdate(delta);
            }
        }
    }
}

void GameObject::OnRender()
{
    if (m_cameras == nullptr) return;
    for (auto camera : *m_cameras) {
        OnRender(*camera);
        OnDraw(*camera);
    }
}

void GameObject::OnRender(const Camera& camera)
{
    if (m_isActive) {
        // 特殊属性处理渲染
        if (m_image) {
            m_image->OnRender(camera);
        }

        if (m_animator) {
            m_animator->OnRender(camera);
        }

        if (m_texts) {
            for (auto& text : *m_texts) {
                text->OnRender(camera);
            }
        }
    }

    if (m_isChildrenActive && m_child_gameObjects) {
        for (auto& game_object : *m_child_gameObjects) {
            if (game_object->GetActive()) {
                game_object->OnRender(camera);
                game_object->OnDraw(camera);
            }
        }
    }
}

void GameObject::AttachCamera(Camera* camera)
{
    if (m_cameras == nullptr) {
        m_cameras = std::make_unique<std::vector<Camera*>>();
    }
    m_cameras->push_back(camera);
}

const GameObject::CameraLists& GameObject::GetCameras()
{
    if (m_parent) {
        return m_parent->GetCameras();
    }
    return m_cameras;
}

bool GameObject::ContainsScreenPoint(const Vector2& pos)
{
    const auto& cameras = GetCameras();
    if (cameras == nullptr) return false;

    if (m_isActive) {
        for (auto camera : *cameras) {
            if (m_image && m_image->ContainsScreenPoint(*camera, pos)) {
                return true;
            }
            if (m_animator && m_animator->ContainsScreenPoint(*camera, pos)) {
                return true;
            }
            if (m_texts) {
                for (const auto& text : *m_texts) {
                    if (text->ContainsScreenPoint(*camera, pos)) return true;
                }
            }
        }
    }

    if (m_isChildrenActive && m_child_gameObjects) {
        for (auto& child : *m_child_gameObjects) {
            if (child->ContainsScreenPoint(pos)) {
                m_containsScreenPointChildObj = child.get();
                return true;
            }
        }
    }

    return false;
}

void GameObject::SetScreenPosition(const Camera& camera, const Vector2& screenPosition)
{
    m_position = screenPosition + camera.GetPosition();
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
        if (m_texts == nullptr) {
            m_texts = std::make_unique<std::vector<TextPtr>>();
        }

        auto text = std::make_unique<Text>(this, std::forward<Args>(args)...);
        auto result = text.get();
        m_texts->emplace_back(std::move(text));
        return result;
    } else {
        static_assert(false, "非法组件创建");
    }

    return nullptr;
}

// 获取组件指针
template <typename T>
std::enable_if_t<std::is_base_of_v<Component, T>, T*> GameObject::GetComponent(int index)
{
    return nullptr;
}
template <>
Image* GameObject::GetComponent<Image>(int index)
{
    return m_image.get();
}
template <>
Animator* GameObject::GetComponent<Animator>(int index)
{
    if (m_animator == nullptr) return nullptr;
    return m_animator.get();
}
template <>
CollisionBox* GameObject::GetComponent<CollisionBox>(int index)
{
    if (m_collisions == nullptr) return nullptr;
    int i = 0;
    for (auto collision : *m_collisions) {
        if (collision->Type() == CollisionType::Box) {
            if (index == i) {
                return static_cast<CollisionBox*>(collision);
            }

            ++i;
        }
    }
    return nullptr;
}

template <>
Rigidbody2D* GameObject::GetComponent<Rigidbody2D>(int index)
{
    if (m_rigidbody2D == nullptr) return nullptr;
    return m_rigidbody2D.get();
}

template <>
Text* GameObject::GetComponent<Text>(int index)
{
    if (m_texts == nullptr) return nullptr;
    int i = 0;
    for (auto& text : *m_texts) {
        if (index == i++) return text.get();
    }
    return nullptr;
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

template Image* GameObject::GetComponent<Image>(int);
template Animator* GameObject::GetComponent<Animator>(int);
template CollisionBox* GameObject::GetComponent<CollisionBox>(int);
template Rigidbody2D* GameObject::GetComponent<Rigidbody2D>(int);
template Text* GameObject::GetComponent<Text>(int);

}  // namespace gameaf