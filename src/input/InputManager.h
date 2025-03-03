#pragma once

/**
 * 用户外设输入信息管理器
 */

#include <bitset>

// 第一个比特位预留, 表示事件检测到的key并没有出现在KeyValue列表中
#define KEY_LEN 30 + 1
// 持续检测
#define INPUT_CONTINUE_INDEX 0
// 当前帧检测 按钮是否触发
#define INPUT_CURRENT_INDEX 1

namespace gameaf {

class InputManager
{
public:
    // 外设消息类型
    enum class MessageType { None, Down, Up };
    // 外设映射类型
    // 键鼠
    enum class KeyValue {
        None,  // 位置key值出现
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
        LButton,
        RButton,
        Space,
        Shift
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

public:
    /// @brief 处理输入
    void ProcessInput();

    /// @brief 清除当前帧记录的事件状态
    void ClearFrameInput() { m_key_state[INPUT_CURRENT_INDEX].reset(); }

private:
    // 双缓冲区, 第一个缓冲区持续检测按键的按下和抬起状态, 第二个缓冲区检测按键是否在当前帧发生
    std::bitset<KEY_LEN> m_key_state[2] = {};
    // TODO: 事件队列?
};
}  // namespace gameaf