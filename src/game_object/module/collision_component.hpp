#pragma once

#include <camera.h>

#include <collision_layer.hpp>
#include <functional>
#include <memory>
#include <vector2.hpp>

/**
 * 碰撞组件模板类
 *
 */

namespace gameaf {

class GameObject;

class CollisionComponent
{
public:
    using ColliideCallback = std::function<void(GameObject&)>;

public:
    Vector2& position() { return m_position; }
    const Vector2& position() const { return m_position; }
    void setOnCollide(ColliideCallback on_collision) { m_on_collide = on_collision; }
    void setSrcLayer(Collisionlayer src_layer) { m_src_layer = src_layer; }
    void addDstLayer(Collisionlayer dst_layer) { Layer::involve(m_dst_layer, dst_layer); }
    const CollisionComponentType& type() const { return m_type; }
    void setEnabled(bool flag) { m_enabled = flag; }
    bool enabled() const { return m_enabled; }

    void setGameObject(GameObject* object) { m_object = object; }
    GameObject* getGameObject() const { return m_object; }
    void detachGameObject() { m_object = nullptr; }

public:
    virtual void processCollide(CollisionComponent* collision_component) = 0;
    virtual void onDebugRender(const Camera& camera) {}

protected:
    GameObject* m_object = nullptr;  // 弱持有状态

    CollisionComponentType m_type = CollisionComponentType::None;
    Vector2 m_position;
    Collisionlayer m_src_layer = Layer::none;  // 当前碰撞组件层级
    Collisionlayer m_dst_layer = Layer::none;  // 目标碰撞组件层级
    ColliideCallback m_on_collide;             // 发生碰撞后触发回调
    bool m_enabled = true;
};
}  // namespace gameaf