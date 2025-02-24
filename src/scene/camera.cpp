#include <camera.h>

#include <log.hpp>

namespace gameaf {

void Camera::onUpdate(float delta)
{
    if (auto obj = m_target_obj.lock()) {
        switch (m_follow_mode) {
            case FollowMode::None:
                // 直接跟随
                lookAt(obj->getPosition());
                break;
            case FollowMode::Smooth:
                // 模拟弹簧
                m_target_position = obj->getPosition() - m_size / 2.0f;
                if (m_pre_target_position != m_target_position) {
                    m_pre_target_position = m_target_position;
                    m_position = Vector2::lerp(m_position, m_target_position, m_smooth_factor);
                }
                break;
            default:
                break;
        }
    }
}
}  // namespace gameaf