#pragma once

#include <macros.h>

#include <cmath>

#include "collision_component.hpp"

/**
 * 碰撞盒组件
 */

namespace gameaf {

class CollisionBox : public CollisionComponent
{
public:
    CollisionBox() { m_type = CollisionComponentType::Box; }
    Vector2& size() { return m_size; }
    const Vector2& size() const { return m_size; }

public:
    void processCollide(CollisionComponent* dst) override
    {
        if (!m_enabled || !dst->enabled()) return;
        // 实现 碰撞盒子的碰撞检测
        if (dst->type() == CollisionComponentType::Box) {
            CollisionBox* dst_box = (CollisionBox*)dst;

            bool is_collide_x =
                max(m_position.X + m_size.X / 2, dst_box->position().X + dst_box->size().X / 2) -
                    min(m_position.X - m_size.X / 2,
                        dst_box->position().X - dst_box->size().X / 2) <=
                (m_size.X + dst_box->size().X);

            bool is_collide_y =
                max(m_position.Y + m_size.Y / 2, dst_box->position().Y + dst_box->size().Y / 2) -
                    min(m_position.Y - m_size.Y / 2,
                        dst_box->position().Y - dst_box->size().Y / 2) <=
                (m_size.Y + dst_box->size().Y);

            if (is_collide_x && is_collide_y) {
                // 碰撞回调调用
                if (m_on_collide) m_on_collide();
                // TODO: 物理更新?
            }
        }
    }

    void onDebugRender(const Camera& camera) override
    {
        int left = static_cast<int>(m_position.X - m_size.X / 2 - camera.getPosition().X);
        int top = static_cast<int>(m_position.Y - m_size.Y / 2 - camera.getPosition().Y);
        int right = static_cast<int>(m_position.X + m_size.X / 2 - camera.getPosition().X);
        int bottom = static_cast<int>(m_position.Y + m_size.Y / 2 - camera.getPosition().Y);
#ifdef GAMEAF_USE_EASYX
        setlinecolor(m_enabled ? RGB(255, 195, 195) : RGB(115, 115, 175));
        rectangle(left, top, right, bottom);
#else
#endif
    }

private:
    Vector2 m_size;
};
}  // namespace gameaf