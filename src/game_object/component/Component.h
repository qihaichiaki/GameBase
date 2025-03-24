#pragma once

#include <common/Vector2.hpp>

/**
 * 组件们的基类, 所有组件继承此类, 用于共享一些共同的属性和方法
 */

namespace gameaf {

class GameObject;
class Camera;
class Component
{
public:
    Component(GameObject* obj, const Vector2& offset);
    virtual ~Component();

public:
    /// @brief 获取组件的相对位移
    const Vector2& GetOffset() const { return m_offset; }

    /// @brief 设置组件的相对位移
    void SetOffset(const Vector2& offset) { m_offset = offset; }

    /// @brief 返回组件的游戏对象
    GameObject* GetGameObject() const { return m_gameObject; }

    /// @brief 设置组件的游戏对象
    virtual void SetGameObject(GameObject* obj) { m_gameObject = obj; }

    /// @brief 组件的世界坐标
    Vector2 Position() const;

    /// @brief 水平翻转组件
    virtual void Flip() { m_offset = Vector2{m_offset.X * -1.0f, m_offset.Y}; };

    /// @brief 判断在对应摄像机下, 窗口坐标是否位于组件上
    virtual bool ContainsScreenPoint(const Camera&, const Vector2&) const { return false; }

    /// @brief 设置是否启动组件
    void SetEnabled(bool flag) { m_enabled = flag; }

    /// @brief 检查是否启动组件
    bool Enabled() const { return m_enabled; }

protected:
    GameObject* m_gameObject = nullptr;  // 弱引用
    Vector2 m_offset;
    bool m_enabled = true;  // 是否启用
};
}  // namespace gameaf