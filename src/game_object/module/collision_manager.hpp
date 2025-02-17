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

    void processCollide()
    {
        for (auto src_collide : m_collide_components) {
            if (!src_collide->enabled()) continue;
            for (auto dst_collide : m_collide_components) {
                if (src_collide == dst_collide) continue;
                src_collide->processCollide(dst_collide);
            }
        }
    }

public:
    CollisionBox* createCollisionBox()
    {
        CollisionBox* collision_box = new CollisionBox();
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