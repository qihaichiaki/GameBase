#include "Camera.h"

#include <GameAf.h>
#include <common/Macros.h>
#include <game_object/GameObject.h>

#include <common/Log.hpp>

namespace gameaf {

Camera::Camera()
{
    m_shakeTimer.SetOneShot(true);
    m_shakeTimer.SetOnTimeout([&]() {
        is_shaking = false;
        m_shakeTimer.Pause();
    });
    m_shakeTimer.Pause();
}
Camera::Camera(Vector2 size) : m_size(size)
{
    m_shakeTimer.SetOneShot(true);
    m_shakeTimer.SetOnTimeout([&]() {
        is_shaking = false;
        m_shakeTimer.Pause();
    });
    m_shakeTimer.Pause();
    m_position = {-m_size.X / 2, -m_size.Y / 2};  // 默认以世界坐标0, 0为中心
}
Camera::~Camera() {}

const Vector2& Camera::GetPosition() const
{
    // 震动时使用震动坐标
    if (is_shaking) return m_shakePosition;
    return m_position;
}

void Camera::SetFollowTarget(std::shared_ptr<GameObject> target_obj, FollowMode mode)
{
    m_target_obj = target_obj.get();
    m_follow_mode = mode;
}

void Camera::SetCameraDeadZone(const Vector2& size)
{
    m_deadZoneOffset = (m_size - size) / 2.0f;
    m_deadZoneSize = size;
}

void Camera::SetCameraDeadZone(const Vector2& offset, const Vector2& size)
{
    m_deadZoneOffset = offset;
    m_deadZoneSize = size;
}

void Camera::Shake(float duration, float shakeIntensity)
{
    m_shakeTimer.SetWaitTime(duration);
    is_shaking = true;
    m_shakeIntensity = shakeIntensity;
    m_shakeTimer.Restart();
}

void Camera::SetFixdDeadZone(const Vector2& fixdDeadZonePos, const Vector2& fixdDeadZoneSize)
{
    m_fixedDeadZonePos = fixdDeadZonePos;
    m_fixedDeadZoneSize = fixdDeadZoneSize;
}

inline Vector2 Camera::__onNormalFollow(float alpha, const Vector2& current_position)
{
    // 正常模式
    switch (m_follow_mode) {
        case FollowMode::None:
            // 直接跟随
            return current_position - m_size / 2.0f - m_position;
            break;
        case FollowMode::Smooth:
            // 平滑跟随
            m_target_position = current_position - m_size / 2.0f;
            return Vector2::Lerp(m_position, m_target_position, m_smooth_factor * alpha) -
                   m_position;
            break;
        default:
            return Vector2{};
            break;
    }
}

inline Vector2 Camera::__onDeadZoneFollow(float alpha, const Vector2& current_position)
{
    // 死区模式
    Vector2 delta = current_position - (m_position + m_deadZoneOffset);
    Vector2 mvDelta;  // 修正delta -> 实际要位移的路径
    mvDelta.X = delta.X < 0                  ? delta.X
                : delta.X > m_deadZoneSize.X ? delta.X - m_deadZoneSize.X
                                             : 0.0f;
    mvDelta.Y = delta.Y < 0                  ? delta.Y
                : delta.Y > m_deadZoneSize.Y ? delta.Y - m_deadZoneSize.Y
                                             : 0.0f;
    switch (m_follow_mode) {
        case FollowMode::None:
            // 直接跟随
            return mvDelta;
            break;
        case FollowMode::Smooth:
            // 平滑跟随
            m_target_position = m_position + mvDelta;
            return Vector2::Lerp(m_position, m_target_position, m_smooth_factor * alpha) -
                   m_position;
            break;
        default:
            return Vector2{};
            break;
    }
}

void Camera::OnFixUpdate(float alpha)
{
    if (m_target_obj) {
        Vector2 current_position = m_target_obj->GetPosition();

        // 固定死区限制
        if (m_fixedDeadZoneSize) {
            current_position.X =
                std::clamp(current_position.X, m_fixedDeadZonePos.X - m_fixedDeadZoneSize.X / 2,
                           m_fixedDeadZonePos.X + m_fixedDeadZoneSize.X / 2);

            current_position.Y =
                std::clamp(current_position.Y, m_fixedDeadZonePos.Y - m_fixedDeadZoneSize.Y / 2,
                           m_fixedDeadZonePos.Y + m_fixedDeadZoneSize.Y / 2);
        }

        Vector2 delta;
        if (m_deadZoneSize != Vector2{}) {
            delta = __onDeadZoneFollow(alpha, current_position);
        } else {
            delta = __onNormalFollow(alpha, current_position);
        }
        m_position += delta;
    }
}

void Camera::OnUpdate(float delta)
{
    m_shakeTimer.OnUpdate(delta);

    if (is_shaking) {
        static auto& gameAF = GameAf::GetInstance();
        m_shakePosition.X =
            (-50 + gameAF.Random<int>(0, 100)) / 50.0f * m_shakeIntensity + m_position.X;
        m_shakePosition.Y =
            (-50 + gameAF.Random<int>(0, 100)) / 50.0f * m_shakeIntensity + m_position.Y;
    }
}

void Camera::OnDebugRender()
{
    // 死区渲染
    if (m_deadZoneSize) {
        int left = static_cast<int>(m_deadZoneOffset.X);
        int top = static_cast<int>(m_deadZoneOffset.Y);
        int right = static_cast<int>(m_deadZoneOffset.X + m_deadZoneSize.X);
        int bottom = static_cast<int>(m_deadZoneOffset.Y + m_deadZoneSize.Y);

#ifdef GAMEAF_USE_EASYX
        setlinecolor(RGB(255, 0, 0));
        rectangle(left, top, right, bottom);
#else
#endif
    }

    // 固定死区
    if (m_fixedDeadZoneSize) {
        Vector2 screenPos = m_fixedDeadZonePos - m_position;
        int left = static_cast<int>(screenPos.X - m_fixedDeadZoneSize.X / 2);
        int top = static_cast<int>(screenPos.Y - m_fixedDeadZoneSize.Y / 2);
        int right = static_cast<int>(screenPos.X + m_fixedDeadZoneSize.X / 2);
        int bottom = static_cast<int>(screenPos.Y + m_fixedDeadZoneSize.Y / 2);

#ifdef GAMEAF_USE_EASYX
        setlinecolor(RGB(0, 0, 255));
        rectangle(left, top, right, bottom);
#else
#endif
    }
}

}  // namespace gameaf