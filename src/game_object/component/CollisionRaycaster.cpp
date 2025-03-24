#include "CollisionRaycaster.h"

#include <common/Macros.h>
#include <scene/Camera.h>

#include <common/Log.hpp>

#include "CollisionBox.h"

namespace gameaf {
CollisionRaycaster::CollisionRaycaster(GameObject* object, const Vector2& offset)
    : Collision(object, offset)
{
    SetType(CollisionType::Ray);
    m_isTrigger = true;
}
CollisionRaycaster::~CollisionRaycaster() {}

void CollisionRaycaster::BuildEndPos()
{
    m_endPos = m_dir == RaycasterDir::Up      ? Vector2{Position().X, Position().Y - m_length}
               : m_dir == RaycasterDir::Down  ? Vector2{Position().X, Position().Y + m_length}
               : m_dir == RaycasterDir::Right ? Vector2{Position().X + m_length, Position().Y}
                                              : Vector2{Position().X - m_length, Position().Y};
}

bool CollisionRaycaster::ProcessCollide(Collision* dst, float delta)
{
    if (!m_enabled || !dst->Enabled()) return false;
    BuildEndPos();
    if (dst->Type() == CollisionType::Box) {
        CollisionBox* dst_box = static_cast<CollisionBox*>(dst);
        Vector2 position = Position();

        float boxTop = dst_box->TopY();
        float boxBottom = dst_box->BottomY();
        float boxLeft = dst_box->LeftX();
        float boxRight = dst_box->RightX();

        if (m_dir == RaycasterDir::Left || m_dir == RaycasterDir::Right) {
            if (position.Y < boxTop || position.Y > boxBottom) return false;
            // x方向上
            float left = min(position.X, m_endPos.X);
            float right = max(position.X, m_endPos.X);

            if (max(right, boxRight) - min(left, boxLeft) < (dst_box->GetSize().X + m_length)) {
                // 触发射线检测
                m_isCollided = true;
                return true;
            }
        } else {
            // y方向上
            if (position.X < boxLeft || position.X > boxRight) return false;

            float top = min(position.Y, m_endPos.Y);
            float bottom = max(position.Y, m_endPos.Y);

            if (max(bottom, boxBottom) - min(top, boxTop) < (dst_box->GetSize().Y + m_length)) {
                // 触发射线检测
                m_isCollided = true;
                return true;
            }
        }
    }

    return false;
}

void CollisionRaycaster::OnDebugRender(const Camera& camera)
{
    BuildEndPos();
#ifdef GAMEAF_USE_EASYX
    setlinecolor(m_enabled ? RGB(255, 195, 195) : RGB(115, 115, 175));
    line(Position().X - camera.GetPosition().X, Position().Y - camera.GetPosition().Y,
         m_endPos.X - camera.GetPosition().X, m_endPos.Y - camera.GetPosition().Y);
#else
#endif
}

void CollisionRaycaster::Flip()
{
    Component::Flip();
    if (m_dir == RaycasterDir::Right || m_dir == RaycasterDir::Left) {
        m_dir = m_dir == RaycasterDir::Right ? RaycasterDir::Left : RaycasterDir::Right;
        BuildEndPos();
    }
}

}  // namespace gameaf