#pragma once

#include "log.hpp"

class GameAF
{
public:
    static GameAF& getInstance();

    // === 游戏主循环内的执行函数 ===

    /// @brief 初始化窗口
    /// @note 切记输出日志前先调用此函数, 否则utf8编码可能出现乱码问题
    void initWindow();

    /// @brief 游戏运行
    /// @note 主线程中阻塞
    void run();

    // === 游戏主循环内的执行函数 ===

    /// === 游戏框架相关参数操作 ===

    /// @brief 设置屏幕的长度和宽度
    void setScreenSize(int width, int height);
    /// @brief 获取屏幕的长度和宽度
    std::tuple<int, int> getScreenSize();
    /// @brief 设置是否显示控制台
    void setShowConsole(bool isShowConsole);
    /// @brief 设置窗口名字
    void setScreenName(const std::string& screenName);
    /// @brief 设置游戏内帧数
    /// @note 设置帧数限制在min ~ max帧的区间内
    void setFPS(int fps);
    /// @brief 设置退出游戏
    void exit();

    /// === 游戏框架相关参数设置 ===

    /// === 游戏日志 ===

    template <typename... Args>
    void log(const std::string& fmt, Args&&... args)
    {
        gameaf::log(fmt, std::forward<Args>(args)...);
    }

    /// === 游戏日志 ===

private:
    GameAF() = default;
    ~GameAF() = default;
    GameAF(const GameAF&) = delete;
    GameAF& operator=(const GameAF&) = delete;

private:
    // 屏幕的长度和宽度 是否显示控制台 窗口名字
    int m_screenWidth = 1280;
    int m_screenHeight = 720;
    bool m_isShowConsole = false;
    std::string m_screenName = "MyGame";

    // 游戏帧数
    int m_fps = 144;

    // 游戏是否退出
    bool m_exit = false;
};