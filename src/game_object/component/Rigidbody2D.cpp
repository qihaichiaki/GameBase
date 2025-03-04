#include "Rigidbody2D.h"

#include <game_object/GameObject.h>

namespace gameaf {
Rigidbody2D::Rigidbody2D(GameObject* game_obj) : m_object(game_obj)
{
    if (game_obj == nullptr) {
        log("[error][Rigidbody2D] gameobject为空!");
    }
}

// 物理循环更新
void Rigidbody2D::OnUpdate(float delta)
{
    m_velocity.Y += m_gravity_scale * delta;
    // 游戏里似乎不用牛顿位移公式, 使用半隐式欧拉公式, 似乎更稳定?
    m_object->Translate(m_velocity * delta);
}
}  // namespace gameaf