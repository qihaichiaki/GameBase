#include "Collision.h"

#include <game_object/GameObject.h>

namespace gameaf {
void Collision::SetOnCollide(ColliideCallback on_collision) { m_on_collide = on_collision; }
void Collision::SetSrcLayer(Collisionlayer src_layer) { m_src_layer = src_layer; }
void Collision::AddDstLayer(Collisionlayer dst_layer)
{
    CollisionLayerTool::Involve(m_dst_layer, dst_layer);
}
bool Collision::CheckLayer(const Collision& collision)
{
    return CollisionLayerTool::IsContain(m_dst_layer, collision.m_src_layer);
}
CollisionType Collision::Type() const { return m_type; }
void Collision::SetType(CollisionType type) { m_type = type; }
void Collision::SetOffset(const Vector2& offset) { m_offset = offset; }
void Collision::SetEnabled(bool flag) { m_enabled = flag; }
bool Collision::Enabled() const { return m_enabled; }
GameObject* Collision::MyObject() const { return m_object; }
Vector2 Collision::GetPosition() const { return m_object->GetPosition() + m_offset; }
}  // namespace gameaf