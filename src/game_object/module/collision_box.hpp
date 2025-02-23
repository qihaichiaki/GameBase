#pragma once

#include <game_object.h>
#include <macros.h>

#include <cmath>

#include "collision_component.hpp"
#include "rigidbody2d.hpp"

/**
 * 碰撞盒组件
 */

namespace gameaf {

class CollisionBox : public CollisionComponent
{
public:
    CollisionBox(GameObject* object) : CollisionComponent(object)
    {
        setType(CollisionComponentType::Box);
    }
    CollisionBox(GameObject* object, const Vector2& offset) : CollisionComponent(object, offset)
    {
        setType(CollisionComponentType::Box);
    }

    Vector2& size() { return m_size; }
    const Vector2& size() const { return m_size; }

public:
    void processCollide(CollisionComponent* dst, float delta) override
    {
        if (!m_enabled || !dst->enabled()) return;
        // 实现 碰撞盒子的碰撞检测
        if (dst->type() == CollisionComponentType::Box) {
            CollisionBox* dst_box = static_cast<CollisionBox*>(dst);
            Vector2 position = getPosition();
            Vector2 dst_position = dst_box->getPosition();

            float right = position.X + m_size.X / 2;
            float left = position.X - m_size.X / 2;
            float dst_left = dst_position.X - dst_box->size().X / 2;
            float dst_right = dst_position.X + dst_box->size().X / 2;
            float bottom = position.Y + m_size.Y / 2;
            float top = position.Y - m_size.Y / 2;
            float dst_bottom = dst_position.Y + dst_box->size().Y / 2;
            float dst_top = dst_position.Y - dst_box->size().Y / 2;

            bool collide_x =
                (max(right, dst_right) - min(left, dst_left)) < (m_size.X + dst_box->size().X);

            bool collide_y =
                (max(bottom, dst_bottom) - min(top, dst_top)) < (m_size.Y + dst_box->size().Y);

            if (collide_x && collide_y) {
                // 碰撞回调调用
                // 将对方的游戏对象传入进去
                if (m_on_collide) {
                    m_on_collide(*dst->getObject());
                }

                // 存在刚体进行物理位置修正
                // 使用mtv(Minimum Translation Vector)最小位移矢量进行位置修正
                if (auto rb = m_object->m_rigidbody2D) {
                    float collided_delta_x = min(right - dst_left, dst_right - left);
                    float collided_delta_y = min(bottom - dst_top, dst_bottom - top);

                    if (collided_delta_x <= collided_delta_y) {
                        rb->velocity().X = 0.0f;
                        getObject()->translate(
                            {left < dst_left ? -collided_delta_x : collided_delta_x, 0.0f});
                    } else {
                        rb->velocity().Y = 0.0f;
                        getObject()->translate(
                            {0.0f, bottom < dst_bottom ? -collided_delta_y : collided_delta_y});
                    }
                }
            }
        }
    }

    void onDebugRender(const Camera& camera) override
    {
        Vector2 position = getPosition();
        int left = static_cast<int>(position.X - m_size.X / 2 - camera.getPosition().X);
        int top = static_cast<int>(position.Y - m_size.Y / 2 - camera.getPosition().Y);
        int right = static_cast<int>(position.X + m_size.X / 2 - camera.getPosition().X);
        int bottom = static_cast<int>(position.Y + m_size.Y / 2 - camera.getPosition().Y);
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