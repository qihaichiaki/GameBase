#include "CollisionBox.h"

#include <common/Macros.h>
#include <game_object/GameObject.h>
#include <game_object/component/Rigidbody2D.h>
#include <scene/Camera.h>

#include <cmath>

namespace gameaf {
void CollisionBox::ProcessCollide(Collision* dst, float delta)
{
    if (!m_enabled || !dst->Enabled()) return;
    // 实现 碰撞盒子的碰撞检测
    if (dst->Type() == CollisionType::Box) {
        CollisionBox* dst_box = static_cast<CollisionBox*>(dst);
        Vector2 position = GetPosition();
        Vector2 dst_position = dst_box->GetPosition();

        float right = position.X + m_size.X / 2;
        float left = position.X - m_size.X / 2;
        float dst_left = dst_position.X - dst_box->GetSize().X / 2;
        float dst_right = dst_position.X + dst_box->GetSize().X / 2;
        float bottom = position.Y + m_size.Y / 2;
        float top = position.Y - m_size.Y / 2;
        float dst_bottom = dst_position.Y + dst_box->GetSize().Y / 2;
        float dst_top = dst_position.Y - dst_box->GetSize().Y / 2;

        bool collide_x =
            (max(right, dst_right) - min(left, dst_left)) < (m_size.X + dst_box->GetSize().X);

        bool collide_y =
            (max(bottom, dst_bottom) - min(top, dst_top)) < (m_size.Y + dst_box->GetSize().Y);

        if (collide_x && collide_y) {
            // 碰撞回调调用
            // 将对方的游戏对象传入进去
            if (m_on_collide) {
                m_on_collide(*(dst->MyObject()));
            }

            // 存在刚体进行物理位置修正
            // 使用mtv(Minimum Translation Vector)最小位移矢量进行位置修正
            if (auto rb = m_object->m_rigidbody2D) {
                float collided_delta_x = min(right - dst_left, dst_right - left);
                float collided_delta_y = min(bottom - dst_top, dst_bottom - top);

                if (collided_delta_x <= collided_delta_y) {
                    rb->Velocity().X = 0.0f;
                    MyObject()->Translate(
                        {left < dst_left ? -collided_delta_x : collided_delta_x, 0.0f});
                } else {
                    rb->Velocity().Y = 0.0f;
                    MyObject()->Translate(
                        {0.0f, bottom < dst_bottom ? -collided_delta_y : collided_delta_y});
                }
            }
        }
    }
}

void CollisionBox::OnDebugRender(const Camera& camera)
{
    Vector2 position = GetPosition();
    int left = static_cast<int>(position.X - m_size.X / 2 - camera.GetPosition().X);
    int top = static_cast<int>(position.Y - m_size.Y / 2 - camera.GetPosition().Y);
    int right = static_cast<int>(position.X + m_size.X / 2 - camera.GetPosition().X);
    int bottom = static_cast<int>(position.Y + m_size.Y / 2 - camera.GetPosition().Y);
#ifdef GAMEAF_USE_EASYX
    setlinecolor(m_enabled ? RGB(255, 195, 195) : RGB(115, 115, 175));
    rectangle(left, top, right, bottom);
#else
#endif
}
}  // namespace gameaf
