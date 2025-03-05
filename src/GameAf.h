#pragma once

#include <random>
#include <string>

class GameAF
{
public:
    static GameAF& GetInstance();

    // === 游戏主循环内的执行函数 ===

    /// @brief 初始化窗口
    /// @note 切记输出日志前先调用此函数, 否则utf8编码可能出现乱码问题
    void InitWindow();

    /// @brief 游戏运行
    /// @note 主线程中阻塞
    void Run();

    // === 游戏主循环内的执行函数 ===

    /// === 游戏框架相关参数操作 ===

    /// @brief 设置屏幕的长度和宽度
    void SetScreenSize(float width, float height);
    /// @brief 获取屏幕的大小
    std::tuple<float, float> GetScreenSize() { return {m_screenWidth, m_screenHeight}; }
    /// @brief 获取屏幕的宽度
    float GetScreenWidth() { return m_screenWidth; }
    /// @brief 获取屏幕的高度
    float GetScreenHeight() { return m_screenHeight; }
    /// @brief 设置是否显示控制台
    void SetShowConsole(bool isShowConsole);
    /// @brief 设置窗口名字
    void SetScreenName(const std::string& screenName);
    /// @brief 设置游戏内帧数
    /// @note 设置帧数限制在min ~ max帧的区间内
    void SetFPS(int fps);
    /// @brief 设置退出游戏
    void Exit();

    /// === 游戏框架相关参数设置 ===

    /// === 游戏框架获取相关参数 ===

    /// @brief 获取当前帧和上一帧的相隔时间, 单位为s
    float GetDeltaTime() { return m_delta_time; }

    /// @brief 生成[start, end]之间的随机数
    /// @warning 如果gameaf没有run, 则生成出来的随机数每次都是固定的
    template <typename T>
    std::enable_if_t<std::is_same_v<T, int> || std::is_same_v<T, float>, T> Random(T start, T end)
    {
    }

    template <>
    int Random<int>(int start, int end)
    {
        std::uniform_int_distribution<int> dist(start, end);
        return dist(gen);
    }

    template <>
    float Random<float>(float start, float end)
    {
        std::uniform_real_distribution<float> dist(start, end);
        return dist(gen);
    }

private:
    GameAF() = default;
    ~GameAF() = default;
    GameAF(const GameAF&) = delete;
    GameAF& operator=(const GameAF&) = delete;

private:
    std::mt19937 gen;

private:
    // 屏幕的长度和宽度 是否显示控制台 窗口名字
    float m_screenWidth = 1280.0f;
    float m_screenHeight = 720.0f;
    bool m_isShowConsole = false;
    std::string m_screenName = "MyGame";
    float m_delta_time;

    // 游戏帧数
    int m_fps = 144;

    // 游戏是否退出
    bool m_exit = false;
};