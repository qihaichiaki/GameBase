#pragma once

#include <game_object/GameObject.h>
#include <game_object/component/CollisionBox.h>
#include <game_object/component/CollisionRaycaster.h>

#include <common/Log.hpp>
#include <unordered_set>
#include <vector>

/**
 * 碰撞管理器
 * 负责创建出各种类型的碰撞组件
 */

namespace gameaf {

class CollisionManager
{
public:
    static CollisionManager& GetInstance()
    {
        static CollisionManager instance;
        return instance;
    }

    void ProcessCollide(float delta)
    {
        for (size_t i = 0; i < m_collide_components.size(); ++i) {
            Collision* src_collide = m_collide_components[i];
            if (!src_collide->Enabled()) continue;
            for (size_t j = i + 1; j < m_collide_components.size(); ++j) {
                Collision* dst_collide = m_collide_components[j];
                if (src_collide == dst_collide || !src_collide->CheckLayer(*dst_collide)) continue;
                CollidedInfo key = {src_collide, dst_collide};
                if (src_collide->ProcessCollide(dst_collide, delta)) {
                    // gameaf::log("{} 和 {} 发生碰撞", src_collide->m_gameObject->GetName(),
                    //             dst_collide->m_gameObject->GetName());
                    if (m_previousCollisions.count(key)) {
                        if (src_collide->m_isTrigger && src_collide->m_onTriggerStay)
                            src_collide->m_onTriggerStay(dst_collide);
                        if (dst_collide->m_isTrigger && dst_collide->m_onTriggerStay)
                            dst_collide->m_onTriggerStay(src_collide);
                        if (!src_collide->m_isTrigger && !dst_collide->m_isTrigger) {
                            if (src_collide->m_onCollideStay)
                                src_collide->m_onCollideStay(dst_collide);
                            if (dst_collide->m_onCollideStay)
                                dst_collide->m_onCollideStay(src_collide);
                        }
                    } else {
                        if (src_collide->m_isTrigger && src_collide->m_onTriggerEnter)
                            src_collide->m_onTriggerEnter(dst_collide);
                        if (dst_collide->m_isTrigger && dst_collide->m_onTriggerEnter)
                            dst_collide->m_onTriggerEnter(src_collide);
                        if (!src_collide->m_isTrigger && !dst_collide->m_isTrigger) {
                            if (src_collide->m_onCollideEnter)
                                src_collide->m_onCollideEnter(dst_collide);
                            if (dst_collide->m_onCollideEnter)
                                dst_collide->m_onCollideEnter(src_collide);
                        }
                        m_previousCollisions.insert(key);
                    }
                } else {
                    if (m_previousCollisions.count(key)) {
                        if (src_collide->m_isTrigger && src_collide->m_onTriggerExit)
                            src_collide->m_onTriggerExit(dst_collide);
                        if (dst_collide->m_isTrigger && dst_collide->m_onTriggerExit)
                            dst_collide->m_onTriggerExit(src_collide);
                        if (!src_collide->m_isTrigger && !dst_collide->m_isTrigger) {
                            if (src_collide->m_onCollideExit)
                                src_collide->m_onCollideExit(dst_collide);
                            if (dst_collide->m_onCollideExit)
                                dst_collide->m_onCollideExit(src_collide);
                        }
                        m_previousCollisions.erase(key);
                    }
                }
            }
        }
    }

    void ClearCollided()
    {
        for (auto collide : m_collide_components) {
            collide->ClearCollided();
        }
    }

public:
    void DeleteCollisionComponent(Collision* del_coll_component)
    {
        if (del_coll_component == nullptr) return;
        m_collide_components.erase(std::remove(m_collide_components.begin(),
                                               m_collide_components.end(), del_coll_component),
                                   m_collide_components.end());

        // 删除碰撞缓存
        // TODO: 是否判断执行缓存回调?
        for (auto it = m_previousCollisions.begin(); it != m_previousCollisions.end();) {
            if (it->first == del_coll_component || it->second == del_coll_component) {
                it = m_previousCollisions.erase(it);
            } else
                ++it;
        }
        delete del_coll_component;
    }

    Collision* CopyCollision(GameObject* game_object, Collision* copy_coll_component)
    {
        if (copy_coll_component == nullptr) return nullptr;
        if (copy_coll_component->Type() == CollisionType::Box) {
            CollisionBox* new_collision_box =
                CreateCollision<CollisionBox>(game_object, copy_coll_component->GetOffset());
            *new_collision_box = *(static_cast<CollisionBox*>(copy_coll_component));
            return new_collision_box;
        }

        return nullptr;
    }

    template <typename T>
    std::enable_if_t<std::is_base_of_v<Collision, T>, T*> CreateCollision(
        GameObject* game_object, const Vector2& offset = Vector2{})
    {
        T* collision = new T(game_object, offset);
        m_collide_components.emplace_back(collision);
        return collision;
    }

private:
    CollisionManager() {}
    CollisionManager(const CollisionManager&) = delete;
    CollisionManager& operator=(const CollisionManager&) = delete;

    ~CollisionManager()
    {
        for (auto collision_component : m_collide_components) {
            delete collision_component;
        }
    }

private:
    std::vector<Collision*> m_collide_components;

    struct CollidedInfo
    {
        Collision* first = nullptr;
        Collision* second = nullptr;

        bool operator==(const CollidedInfo& other) const
        {
            return (first == other.first && second == other.second);
        }
    };

    // 自定义哈希函数
    struct CollidedInfoHash
    {
        size_t operator()(const CollidedInfo& info) const
        {
            auto h1 = std::hash<Collision*>{}(info.first);
            auto h2 = std::hash<Collision*>{}(info.second);
            return h1 ^ (h2 << 1);  // 混合两个指针的哈希值
        }
    };

    // 缓存是否碰撞信息
    std::unordered_set<CollidedInfo, CollidedInfoHash> m_previousCollisions;
};
}  // namespace gameaf