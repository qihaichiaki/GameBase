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

public:
    void ProcessCollide(Collision* dst, float delta) override;

    void OnDebugRender(const Camera& camera) override;

private:
    Vector2 m_size;
};
}  // namespace gameaf