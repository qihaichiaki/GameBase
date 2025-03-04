#include "Camera.h"

#include <game_object/GameObject.h>

namespace gameaf {

void Camera::OnUpdate(float delta)
{
    if (auto obj = m_target_obj.lock()) {
        switch (m_follow_mode) {
            case FollowMode::None:
                // 直接跟随
                LookAt(obj->GetPosition());
                break;
            case FollowMode::Smooth:
                // 模拟弹簧
                m_target_position = obj->GetPosition() - m_size / 2.0f;
                m_position = Vector2::lerp(m_position, m_target_position, m_smooth_factor);
                break;
            default:
                break;
        }
    }
}
}  // namespace gameaf