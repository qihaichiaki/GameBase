#pragma once

#include <game_object/component/Component.h>

#include <common/Log.hpp>
#include <common/Vector2.hpp>

/**
 * rigidbody2d 组件
 * 物理模拟的组件
 * 存在此组件会进行物理修正
 */

namespace gameaf {

class GameObject;

class Rigidbody2D : public Component
{
public:
    Rigidbody2D(GameObject* obj);

public:
    // 物理循环更新
    void OnFixedUpdate(float alpha);

public:
    /// @brief 刚体的速度
    Vector2& Velocity() { return m_velocity; }

    /// @brief 刚体的物理强度
    float& GravityScale() { return m_gravity_scale; }

private:
    Vector2 m_velocity;             // 速率
    float m_gravity_scale = 980.5;  // 重力加速度规模
};
}  // namespace gameaf