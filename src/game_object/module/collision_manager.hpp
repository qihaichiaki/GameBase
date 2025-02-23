#pragma once

#include <vector>

#include "collision_box.hpp"

/**
 * 碰撞管理器
 * 负责创建出各种类型的碰撞组件
 */

namespace gameaf {

class CollisionManager
{
public:
    static CollisionManager& getInstance()
    {
        static CollisionManager instance;
        return instance;
    }

    void processCollide(float delta)
    {
        for (auto src_collide : m_collide_components) {
            if (!src_collide->enabled()) continue;
            for (auto dst_collide : m_collide_components) {
                if (src_collide == dst_collide) continue;
                src_collide->processCollide(dst_collide, delta);
            }
        }
    }

public:
    void deleteCollisionComponent(CollisionComponent* del_coll_component)
    {
        if (del_coll_component == nullptr) return;
        m_collide_components.erase(std::remove(m_collide_components.begin(),
                                               m_collide_components.end(), del_coll_component),
                                   m_collide_components.end());
        delete del_coll_component;
    }

    CollisionComponent* copyCollisionComponent(CollisionComponent* copy_coll_component)
    {
        if (copy_coll_component == nullptr) return nullptr;
        if (copy_coll_component->type() == CollisionComponentType::Box) {
            CollisionBox* new_collision_box = createCollisionBox(copy_coll_component->getObject());
            *new_collision_box = *(static_cast<CollisionBox*>(copy_coll_component));
            return new_collision_box;
        }

        return nullptr;
    }

    CollisionBox* createCollisionBox(GameObject* game_object)
    {
        CollisionBox* collision_box = new CollisionBox(game_object);
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
    std::vector<CollisionComponent*> m_collide_components;
};
}  // namespace gameaf