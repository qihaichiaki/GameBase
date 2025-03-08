#pragma once

#include <game_object/component/CollisionBox.h>

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
        for (auto src_collide : m_collide_components) {
            if (!src_collide->Enabled()) continue;
            for (auto dst_collide : m_collide_components) {
                if (src_collide == dst_collide || !src_collide->CheckLayer(*dst_collide)) continue;
                src_collide->ProcessCollide(dst_collide, delta);
            }
        }
    }

public:
    void DeleteCollisionComponent(Collision* del_coll_component)
    {
        if (del_coll_component == nullptr) return;
        m_collide_components.erase(std::remove(m_collide_components.begin(),
                                               m_collide_components.end(), del_coll_component),
                                   m_collide_components.end());
        delete del_coll_component;
    }

    Collision* CopyCollision(GameObject* game_object, Collision* copy_coll_component)
    {
        if (copy_coll_component == nullptr) return nullptr;
        if (copy_coll_component->Type() == CollisionType::Box) {
            CollisionBox* new_collision_box =
                CreateCollisionBox(game_object, copy_coll_component->GetOffset());
            *new_collision_box = *(static_cast<CollisionBox*>(copy_coll_component));
            return new_collision_box;
        }

        return nullptr;
    }

    CollisionBox* CreateCollisionBox(GameObject* game_object, const Vector2& offset = Vector2{})
    {
        CollisionBox* collision_box = new CollisionBox(game_object, offset);
        m_collide_components.emplace_back(collision_box);
        return collision_box;
    }

private:
    CollisionManager() = default;
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
};
}  // namespace gameaf