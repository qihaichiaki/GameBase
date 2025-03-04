#pragma once

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
class Collision
{
public:
    using ColliideCallback = std::function<void(GameObject&)>;

    Collision(GameObject* game_object) : m_object(game_object) {}
    Collision(GameObject* game_object, const Vector2& offset)
        : m_object(game_object), m_offset(offset)
    {
    }

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

    /// @brief 设置碰撞体对象相对于游戏对象的偏移
    /// @param offset 偏移量
    void SetOffset(const Vector2& offset);

    /// @brief 设置是否启动碰撞
    void SetEnabled(bool flag);

    /// @brief 检查是否启动碰撞
    bool Enabled() const;

    /// @brief 获取碰撞体对应的游戏对象
    GameObject* MyObject() const;

    /// @brief 获取碰撞体的中心位置
    Vector2 GetPosition() const;

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
    GameObject* m_object = nullptr;  // 弱持有状态
    Vector2 m_offset;                // 和游戏对象之间的坐标差距

    CollisionType m_type = CollisionType::None;
    Collisionlayer m_src_layer = CollisionLayerTool::none;  // 当前碰撞组件层级
    Collisionlayer m_dst_layer = CollisionLayerTool::none;  // 目标碰撞组件层级
    ColliideCallback m_on_collide;                          // 发生碰撞后触发回调
    bool m_enabled = true;
};
}  // namespace gameaf