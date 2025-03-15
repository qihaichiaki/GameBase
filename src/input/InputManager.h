#pragma once

/**
 * 用户外设输入信息管理器
 */

#include <bitset>
#include <common/Vector2.hpp>
#include <memory>
#include <vector>

// 持续检测
#define INPUT_CONTINUE_INDEX 0
// 当前帧检测 按钮是否触发
#define INPUT_CURRENT_INDEX 1

namespace gameaf {
class GameObject;
class InputManager
{
public:
    // 外设消息类型
    enum class MessageType { None, Down, Up };
    // 外设映射类型
    // 键鼠
    enum class KeyValue {
        None,  // 位置key值出现
        _0,
        _1,
        _2,
        _3,
        _4,
        _5,
        _6,
        _7,
        _8,
        _9,
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        LButton,  // 鼠标左键
        RButton,  // 鼠标右键
        Space,
        Shift,
        OEM_MINUS,  // 主键盘上的-
        OEM_PLUS,   // 主键盘上的+
        Count
    };

public:
    static InputManager& GetInstance();

    /// @brief 在当前帧检测`key`是否被按下
    bool IsKeyDown(KeyValue key)
    {
        return m_key_state[INPUT_CURRENT_INDEX][(size_t)key] &
               m_key_state[INPUT_CONTINUE_INDEX][(size_t)key];
    }
    /// @brief 在当前帧检测`key`是否被松开
    bool IsKeyUp(KeyValue key)
    {
        // 先判断当前帧按键是否触发, 然后判断按键是否松开
        return m_key_state[INPUT_CURRENT_INDEX][(size_t)key] &
               (m_key_state[INPUT_CURRENT_INDEX][(size_t)key] ^
                m_key_state[INPUT_CONTINUE_INDEX][(size_t)key]);
    }

    /// @brief 检测`key`是否被按下
    bool GetKey(KeyValue key) { return m_key_state[INPUT_CONTINUE_INDEX].test((size_t)key); }

    const Vector2& GetMousePos() { return m_mousePos; }

public:
    /// @brief 处理输入
    void ProcessInput();

    /// @brief 清除当前帧记录的事件状态
    void ClearFrameInput() { m_key_state[INPUT_CURRENT_INDEX].reset(); }

    /// @brief 清理场景中的缓存
    /// @param obj 指定缓存对象, 如果设置为null, 则直接清理
    void ClearSenseCache(GameObject* obj = nullptr);

    /// @brief 处理游戏对象鼠标事件
    /// @param objs 游戏对象们, 需要render从小到大(适配scene层处理gameobjec的模式)
    void ProcessMouseEvent(const std::vector<std::shared_ptr<GameObject>>& objs);

private:
    constexpr static size_t KEY_VALUE_LENS = static_cast<size_t>(KeyValue::Count);
    // 双缓冲区, 第一个缓冲区持续检测按键的按下和抬起状态, 第二个缓冲区检测按键是否在当前帧发生
    std::bitset<KEY_VALUE_LENS> m_key_state[2] = {};
    // TODO: 事件队列?

    // 鼠标位置
    Vector2 m_mousePos;
};
}  // namespace gameaf