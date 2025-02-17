#include "../include/game_af.h"

#include <input_manager.h>
#include <macros.h>
#include <scene_manager.h>

#include <chrono>
#include <codecvt>
#include <module/collision_manager.hpp>
#include <thread>

using namespace gameaf;

GameAF& GameAF::getInstance()
{
    static GameAF instance;
    return instance;
}

void GameAF::initWindow()
{
    // 初始化屏幕
    WINDOWS_USE_UTF8;
#ifdef GAMEAF_USE_EASYX
    HWND hwnd = m_isShowConsole ? initgraph(m_screenWidth, m_screenHeight, EX_SHOWCONSOLE)
                                : initgraph(m_screenWidth, m_screenHeight);

    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    SetWindowTextW(hwnd, convert.from_bytes(m_screenName).c_str());
#else
#endif
}

void GameAF::run()
{
    using std::chrono::duration;
    using std::chrono::nanoseconds;
    using std::chrono::steady_clock;

    // 计算每帧运行的时间
    nanoseconds frame_duration((int)1e9 / m_fps);
    // 记录当前时刻
    steady_clock::time_point last_tick = steady_clock::now();
    steady_clock::time_point start_tick;

#ifdef GAMEAF_USE_EASYX
    BeginBatchDraw();
#else
#endif

    while (!m_exit) {
        // 处理消息
        InputManager::getInstance().processInput();

        // 计算当前帧经过时间
        start_tick = steady_clock::now();
        auto delta = duration<float>(start_tick - last_tick);
        m_delta_time = delta.count();
        last_tick = start_tick;

        // 场景数据更新
        SceneManager::getInstance().onUpdate(m_delta_time);
        // TODO: 全局游戏对象数据更新

        // 碰撞管理器更新
        CollisionManager::getInstance().processCollide();

#ifdef GAMEAF_USE_EASYX
        setbkcolor(RGB(0, 0, 0));
        cleardevice();
#else
#endif

        // 渲染更新
        SceneManager::getInstance().onRender();
        // TODO: 全局游戏对象渲染更新

#ifdef GAMEAF_USE_EASYX
        FlushBatchDraw();
#else
#endif
        // 清除帧间消息缓存
        InputManager::getInstance().clearFrameInput();

        // 计算当前帧多余的时间进行sleep
        auto sleep_delta = frame_duration - (steady_clock::now() - last_tick);
        if (sleep_delta > nanoseconds(0)) {
            std::this_thread::sleep_for(sleep_delta);
        }
    }
#ifdef GAMEAF_USE_EASYX
    EndBatchDraw();
    closegraph();
#else
#endif
}

void GameAF::setScreenSize(float width, float height)
{
    m_screenWidth = width;
    m_screenHeight = height;
}

void GameAF::setShowConsole(bool isShowConsole) { m_isShowConsole = isShowConsole; }

void GameAF::setScreenName(const std::string& screenName) { m_screenName = screenName; };

void GameAF::setFPS(int fps)
{
    m_fps = fps < GAMEAF_MIN_FPS ? GAMEAF_MIN_FPS : fps > GAMEAF_MAX_FPS ? GAMEAF_MAX_FPS : fps;
}

void GameAF::exit() { m_exit = true; }
