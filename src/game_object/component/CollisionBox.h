#pragma once

#include <game_object/component/Collision.h>

/**
 * 碰撞盒组件
 */

namespace gameaf {

class CollisionBox : public Collision
{
public:
    CollisionBox(GameObject* object, const Vector2& offset = Vector2{});
    ~CollisionBox();

public:
    const Vector2& GetSize() const { return m_size; }
    void SetSize(const Vector2& size) { m_size = size; }

    // 碰撞盒aabb式
    float LeftX() const { return Position().X - m_size.X / 2; }
    float RightX() const { return Position().X + m_size.X / 2; }
    float TopY() const { return Position().Y - m_size.Y / 2; }
    float BottomY() const { return Position().Y + m_size.Y / 2; }

public:
    bool ProcessCollide(Collision* dst, float delta) override;

    void OnDebugRender(const Camera& camera) override;

private:
    Vector2 m_size;
};
}  // namespace gameaf