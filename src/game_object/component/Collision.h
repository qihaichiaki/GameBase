#pragma once

#include <game_object/component/Component.h>

#include <common/Vector2.hpp>
#include <functional>
#include <game_object/component/CollisionLayer.hpp>
#include <memory>
/**
 * 碰撞组件模板类
 *
 */

namespace gameaf {

class GameObject;
class Camera;
class Collision : public Component
{
public:
    using ColliideCallback = std::function<void(Collision*)>;

    Collision(GameObject* game_object, const Vector2& offset);
    ~Collision();

public:
    /// @brief 设置碰撞回调
    /// @param on_collision 碰撞后的执行函数
    void SetOnCollide(ColliideCallback on_collision);

    /// @brief 设置当前碰撞体的层级
    /// @param src_layer 碰撞层级
    void SetSrcLayer(Collisionlayer src_layer);

    /// @brief 增加目标碰撞体的层级
    /// @param dst_layer 碰撞层级
    void AddDstLayer(Collisionlayer dst_layer);

    /// @brief 检查两碰撞器层级是否需要检测
    /// @param collision 目标碰撞器
    /// @return 是否检测
    bool CheckLayer(const Collision& collision);

    /// @brief 获取碰撞体类型
    CollisionType Type() const;

    /// @brief 设置是否启动碰撞
    void SetEnabled(bool flag);

    /// @brief 检查是否启动碰撞
    bool Enabled() const;

    /// @brief 是否触发碰撞
    bool IsCollided() const { return m_isCollided; }

    /// @brief 清除碰撞状态
    void ClearCollided() { m_isCollided = false; }

public:
    /// @brief 碰撞检测和修正
    /// @param collision_component 对方的碰撞器
    /// @param delta
    virtual void ProcessCollide(Collision* collision_component, float delta) = 0;

    /// @brief 通过摄像机debug渲染碰撞盒子
    virtual void OnDebugRender(const Camera& camera) {}

protected:
    /// @brief 设置碰撞体类型
    void SetType(CollisionType type);

protected:
    CollisionType m_type = CollisionType::None;
    Collisionlayer m_src_layer = CollisionLayerTool::none;  // 当前碰撞组件层级
    Collisionlayer m_dst_layer = CollisionLayerTool::none;  // 目标碰撞组件层级
    ColliideCallback m_on_collide;                          // 发生碰撞后触发回调
    bool m_enabled = true;
    bool m_isCollided = false;  // 是否触发碰撞
};
}  // namespace gameaf