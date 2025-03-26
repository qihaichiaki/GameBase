#include "Rigidbody2D.h"

#include <game_object/GameObject.h>

namespace gameaf {
Rigidbody2D::Rigidbody2D(GameObject* obj) : Component(obj, Vector2{}) {}

// 物理循环更新
void Rigidbody2D::OnFixedUpdate(float alpha)
{
    // 根据是否重力模拟
    if (isGravityEnabled) m_velocity.Y += m_gravity_scale * alpha;
    // 游戏里似乎不用牛顿位移公式, 使用半隐式欧拉公式, 似乎更稳定?
    m_gameObject->Translate(m_velocity * alpha);
}
}  // namespace gameaf