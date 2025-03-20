#pragma once

#include <game_object/component/Collision.h>

/**
 * 射线碰撞检测
 * 用于辅助游戏中的一些行为检测
 * 当前的实现基于碰撞盒AABB实现模式实现, 并未引入旋转(等待后续补充)
 */

namespace gameaf {

enum class RaycasterDir { Up, Down, Left, Right };

class CollisionRaycaster : public Collision
{
public:
    CollisionRaycaster(GameObject* object, const Vector2& offset = Vector2{});
    ~CollisionRaycaster();

public:
    bool ProcessCollide(Collision* dst, float delta) override;
    void OnDebugRender(const Camera& camera) override;
    void Flip() override;

public:
    /// @brief 设置射线长度
    void SetLength(float len) { m_length = len; }

    /// @brief 设置射线方向
    void SetDir(RaycasterDir dir) { m_dir = dir; }

private:
    void BuildEndPos();

private:
    float m_length = 3.0f;                     // 射线长度
    RaycasterDir m_dir = RaycasterDir::Right;  // 射线方向
    Vector2 m_endPos;                          // 射线的最终点坐标
};

}  // namespace gameaf