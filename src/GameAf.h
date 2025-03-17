#pragma once

#include <random>
#include <string>

class GameAf
{
public:
    static GameAf& GetInstance();

    // === 游戏主循环内的执行函数 ===

    /// @brief 初始化窗口,游戏
    /// @param iconPath 窗口图标路径
    /// @param isShowConsole 是否显示控制台
    /// @param screenName 窗口名称
    /// @param screenWidth 屏幕宽度
    /// @param screenHeight 屏幕高度
    /// @warning ⚠使用gameaf框架构建游戏时, 优先使用此函数(前面不可使用此框架内的其他东西), 否则容易崩溃(乱码, 释放资源顺序不正确)
    static void InitWindow(const std::string& iconPath = "", bool isShowConsole = false,
                           const std::string& screenName = "MyGame", int screenWidth = 1280,
                           int screenHeight = 720);

    /// @brief 游戏运行
    /// @note 主线程中阻塞
    void Run();

    // === 游戏主循环内的执行函数 ===

    /// === 游戏框架相关参数操作 ===

    /// @brief 获取屏幕的大小
    static std::tuple<int, int> GetScreenSize();
    /// @brief 获取屏幕的宽度
    static int GetScreenWidth();
    /// @brief 获取屏幕的高度
    static int GetScreenHeight();
    /// @brief 设置游戏内帧数
    /// @note 设置帧数限制在min ~ max帧的区间内
    void SetFPS(int fps);
    /// @brief 设置退出游戏
    void Exit();

    /// === 游戏框架相关参数设置 ===

    /// === 游戏框架获取相关参数 ===

    /// @brief 获取当前帧和上一帧的相隔时间, 单位为s
    float GetDeltaTime() { return m_delta_time; }

    /// @brief 获取固定的物理频率
    float GetFixDeltaTime() { return m_fixed_timestep; }

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
    GameAf() = default;
    ~GameAf() = default;
    GameAf(const GameAf&) = delete;
    GameAf& operator=(const GameAf&) = delete;

private:
    std::mt19937 gen;

private:
    float m_delta_time;
    float m_fixed_timestep = 1.0f / 60.0f;  // 物理固定频率 - 60hz

    // 游戏帧数
    int m_fps = 144;

    // 游戏是否退出
    bool m_exit = false;
};