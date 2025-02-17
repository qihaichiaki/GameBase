#pragma once

#include <camera.h>

#include <collision_layer.hpp>
#include <functional>
#include <vector2.hpp>

/**
 * 碰撞组件模板类
 *
 */

namespace gameaf {

class CollisionComponent
{
public:
    Vector2& position() { return m_position; }
    const Vector2& position() const { return m_position; }
    void setOnCollide(std::function<void()> on_collision) { m_on_collide = on_collision; }
    void setSrcLayer(Collisionlayer src_layer) { m_src_layer = src_layer; }
    void addDstLayer(Collisionlayer dst_layer) { Layer::involve(m_dst_layer, dst_layer); }
    const CollisionComponentType& type() const { return m_type; }
    void setEnabled(bool flag) { m_enabled = flag; }
    bool enabled() const { return m_enabled; }

public:
    virtual void processCollide(CollisionComponent* collision_component) = 0;
    virtual void onDebugRender(const Camera& camera) {}

protected:
    CollisionComponentType m_type = CollisionComponentType::None;
    Vector2 m_position;
    Collisionlayer m_src_layer = Layer::none;  // 当前碰撞组件层级
    Collisionlayer m_dst_layer = Layer::none;  // 目标碰撞组件层级
    std::function<void()> m_on_collide;        // 发生碰撞后触发回调
    bool m_enabled = true;
};
}  // namespace gameaf