#include "Collision.h"

#include <game_object/GameObject.h>

namespace gameaf {

Collision::Collision(GameObject* game_object, const Vector2& offset)
    : Component(game_object, offset)
{
}
Collision::~Collision() {}

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
}  // namespace gameaf