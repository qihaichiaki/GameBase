#pragma once

#include <common/Log.hpp>
#include <common/Vector2.hpp>

/**
 * rigidbody2d 组件
 * 物理模拟的组件
 * 存在此组件会进行物理修正
 */

namespace gameaf {

class GameObject;

class Rigidbody2D
{
public:
    Rigidbody2D(GameObject* game_obj);

public:
    // 物理循环更新
    void OnUpdate(float delta);

    Vector2& Velocity() { return m_velocity; }
    float& GravityScale() { return m_gravity_scale; }

private:
    GameObject* m_object = nullptr;  // 游戏对象弱持有状态
    Vector2 m_velocity;              // 速率
    float m_gravity_scale = 980.5;   // 重力加速度规模
};
}  // namespace gameaf