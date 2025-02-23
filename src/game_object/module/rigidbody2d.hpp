#pragma once

#include <game_object.h>

#include <log.hpp>
#include <vector2.hpp>

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
    Rigidbody2D(GameObject* game_obj) : m_object(game_obj)
    {
        if (game_obj == nullptr) {
            log("[error][Rigidbody2D] gameobject为空!");
        }
    }

public:
    // 物理循环更新
    void onUpdate(float delta)
    {
        m_velocity.Y += m_gravity_scale * delta;
        // 游戏里似乎不用牛顿位移公式, 使用半隐式欧拉公式, 似乎更稳定?
        m_object->translate(m_velocity * delta);
    }

    Vector2& velocity() { return m_velocity; }
    float& gravityScale() { return m_gravity_scale; }

private:
    GameObject* m_object = nullptr;  // 游戏对象弱持有状态
    Vector2 m_velocity;              // 速率
    float m_gravity_scale = 980.5;   // 重力加速度规模
};
}  // namespace gameaf