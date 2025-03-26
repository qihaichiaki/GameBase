#include "CollisionBox.h"

#include <common/Macros.h>
#include <game_object/GameObject.h>
#include <game_object/component/Rigidbody2D.h>
#include <scene/Camera.h>

#include <cmath>

namespace gameaf {

CollisionBox::CollisionBox(GameObject* object, const Vector2& offset) : Collision(object, offset)
{
    SetType(CollisionType::Box);
}

CollisionBox::~CollisionBox() {}

// 物理修正，解决冲突
static inline bool ResolveCollisions(GameObject* gameObject, float left, float right,
                                     float dst_left, float dst_right, float top, float bottom,
                                     float dst_top, float dst_bottom)
{
    // 存在刚体进行物理位置修正
    // 使用mtv(Minimum Translation Vector)最小位移矢量进行位置修正
    if (auto rb = gameObject->GetComponent<Rigidbody2D>()) {
        float collided_delta_x = min(right - dst_left, dst_right - left);
        float collided_delta_y = min(bottom - dst_top, dst_bottom - top);

        if (collided_delta_x <= collided_delta_y) {
            rb->Velocity().X = 0.0f;
            gameObject->Translate({left < dst_left ? -collided_delta_x : collided_delta_x, 0.0f});
        } else {
            rb->Velocity().Y = 0.0f;
            gameObject->Translate(
                {0.0f, bottom < dst_bottom ? -collided_delta_y : collided_delta_y});
        }
        return true;
    }
    return false;
}

bool CollisionBox::ProcessCollide(Collision* dst, float delta)
{
    if (!m_enabled || !dst->Enabled()) return false;
    // 实现 碰撞盒子的碰撞检测
    if (dst->Type() == CollisionType::Box) {
        CollisionBox* dst_box = static_cast<CollisionBox*>(dst);
        Vector2 position = Position();
        Vector2 dst_position = dst_box->Position();

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
            m_isCollided = true;
            dst_box->m_isCollided = true;
            // if (dst_box->m_gameObject->GetName() == "GameObject") {
            //     gameaf::log("size: {}", dst_box->GetSize());
            // }

            if (m_isTrigger || dst_box->m_isTrigger) return true;
            // 本身存在刚体解决冲突
            if (!ResolveCollisions(m_gameObject, left, right, dst_left, dst_right, top, bottom,
                                   dst_top, dst_bottom)) {
                // 如果当前没修正成功, 尝试另一个对象
                ResolveCollisions(dst_box->m_gameObject, dst_left, dst_right, left, right, dst_top,
                                  dst_bottom, top, bottom);  // 注意需要反过来
            }
            return true;
        }
    } else if (dst->Type() == CollisionType::Ray) {
        return dst->ProcessCollide(this, delta);
    }
    return false;
}

void CollisionBox::OnDebugRender(const Camera& camera)
{
    Vector2 position = Position();
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
