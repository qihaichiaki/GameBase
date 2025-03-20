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
    friend class CollisionManager;

public:
    using ColliideCallback = std::function<void(Collision*)>;

    Collision(GameObject* game_object, const Vector2& offset);
    ~Collision();

public:
    /// @brief 设置碰撞回调, 发生碰撞后会调用的函数, 碰撞过程中只会调用一次
    /// @param on_collision 碰撞后的执行函数
    void SetOnCollideEnter(ColliideCallback on_collision) { m_onCollideEnter = on_collision; }

    /// @brief 设置碰撞回调, 发生碰撞后会调用的函数
    /// @param on_collision 碰撞后的执行函数
    void SetOnCollideStay(ColliideCallback on_collision) { m_onCollideStay = on_collision; }

    /// @brief 设置碰撞回调, 碰撞退出后会调用的函数
    /// @param on_collision 碰撞结束后的执行函数
    void SetOnCollideExit(ColliideCallback on_collision) { m_onCollideExit = on_collision; }

    /// @brief 设置触发器回调, 发生碰撞后会调用的函数, 碰撞过程中只会触发一次
    /// @param on_collision 碰撞后的执行函数
    void SetOnTriggerEnter(ColliideCallback on_collision) { m_onTriggerEnter = on_collision; }

    /// @brief 设置触发器回调, 发生碰撞后会调用的函数
    /// @param on_collision 碰撞后的执行函数
    void SetOnTriggerStay(ColliideCallback on_collision) { m_onTriggerStay = on_collision; }

    /// @brief 设置触发器回调, 碰撞退出后会调用的函数
    /// @param on_collision 碰撞结束后的执行函数
    void SetOnTriggerExit(ColliideCallback on_collision) { m_onTriggerExit = on_collision; }

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

    /// @brief 设置是否触发器
    /// @note 如果设置为触发器, 则不会影响物理行为, 只触发回调
    void SetTrigger(bool isTrigger) { m_isTrigger = isTrigger; }

public:
    /// @brief 碰撞检测和修正
    /// @param collision_component 对方的碰撞器
    /// @param delta
    /// @return 返回当前检测是否碰撞
    virtual bool ProcessCollide(Collision* collision_component, float delta) = 0;

    /// @brief 通过摄像机debug渲染碰撞盒子
    virtual void OnDebugRender(const Camera& camera) {}

protected:
    /// @brief 设置碰撞体类型
    void SetType(CollisionType type);

protected:
    CollisionType m_type = CollisionType::None;
    Collisionlayer m_src_layer = CollisionLayerTool::none;  // 当前碰撞组件层级
    Collisionlayer m_dst_layer = CollisionLayerTool::none;  // 目标碰撞组件层级
    ColliideCallback m_onCollideEnter;                      // 发生碰撞后触发一次回调
    ColliideCallback m_onCollideStay;                       // 发生碰撞后持续触发回调
    ColliideCallback m_onCollideExit;                       // 碰撞离开后触发回调
    ColliideCallback m_onTriggerEnter;                      // 触发后回调
    ColliideCallback m_onTriggerStay;                       // 触发后持续回调
    ColliideCallback m_onTriggerExit;                       // 触发离开发生回调
    bool m_enabled = true;
    bool m_isCollided = false;  // 是否触发碰撞
    bool m_isTrigger = false;   // 是否触发器
};
}  // namespace gameaf