#pragma once

#include <functional>

/**
 * 基于帧之间delta时间更新的计时器, 被动画类使用用于更新
 * 动画帧数
 */

namespace gameaf {

class Timer
{
public:
    void OnUpdate(float delta)
    {
        if (m_paused) return;
        m_current_time += delta;
        if (m_current_time >= m_wait_time) {
            m_current_time = 0;

            if (m_one_shot && !m_shotted || !m_one_shot) {
                if (m_timeout) m_timeout();
                m_shotted = true;
            }
        }
    }

    /// @brief 重新开始
    void Restart()
    {
        m_current_time = 0;
        m_shotted = false;
    }

    /// @brief 设置计时器等待时间
    /// @param val 等待时间s
    void SetWaitTime(float val) { m_wait_time = val; }

    /// @brief 单次触发回调
    /// @param flag 是否单次触发
    void SetOneShot(bool flag) { m_one_shot = flag; }

    /// @brief 设置回调
    /// @param onTimeout void()
    void SetOnTimeout(std::function<void()> onTimeout) { m_timeout = onTimeout; }

    /// @brief 暂停
    void Pause() { m_paused = true; }

    /// @brief 继续
    void Resume() { m_paused = false; }

private:
    float m_current_time = 0;         // 当前的时间
    float m_wait_time = 0;            // 等待的时间
    bool m_paused = false;            // 是否暂停
    bool m_shotted = false;           // 回调是否触发过一次
    bool m_one_shot = false;          // 是否只单次触发回调
    std::function<void()> m_timeout;  // 等待时间到的回调函数
};
}  // namespace gameaf