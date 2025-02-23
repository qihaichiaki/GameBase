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

    CollisionComponent(GameObject* game_object) : m_object(game_object) {}
    CollisionComponent(GameObject* game_object, const Vector2& offset)
        : m_object(game_object), m_offset(offset)
    {
    }

public:
    void setOnCollide(ColliideCallback on_collision) { m_on_collide = on_collision; }
    void setSrcLayer(Collisionlayer src_layer) { m_src_layer = src_layer; }
    void addDstLayer(Collisionlayer dst_layer) { Layer::involve(m_dst_layer, dst_layer); }
    const CollisionComponentType& type() const { return m_type; }
    void setType(CollisionComponentType type) { m_type = type; }
    void setOffset(const Vector2& offset) { m_offset = offset; }
    void setEnabled(bool flag) { m_enabled = flag; }
    bool enabled() const { return m_enabled; }
    GameObject* getObject() const { return m_object; }
    Vector2 getPosition() const { return m_object->getPosition() + m_offset; }

public:
    virtual void processCollide(CollisionComponent* collision_component, float delta) = 0;
    virtual void onDebugRender(const Camera& camera) {}

protected:
    GameObject* m_object = nullptr;  // 弱持有状态
    Vector2 m_offset;                // 和游戏对象之间的坐标差距

    CollisionComponentType m_type = CollisionComponentType::None;
    Collisionlayer m_src_layer = Layer::none;  // 当前碰撞组件层级
    Collisionlayer m_dst_layer = Layer::none;  // 目标碰撞组件层级
    ColliideCallback m_on_collide;             // 发生碰撞后触发回调
    bool m_enabled = true;
};
}  // namespace gameaf