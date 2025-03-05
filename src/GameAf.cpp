#include "GameAf.h"

#include <common/Macros.h>
#include <input/InputManager.h>
#include <scene/SceneManager.h>

#include <chrono>
#include <codecvt>
#include <game_object/component/CollisionManager.hpp>
#include <game_object/component/RigidbodyManager.hpp>
#include <thread>

using namespace gameaf;

GameAF& GameAF::GetInstance()
{
    static GameAF instance;
    return instance;
}

void GameAF::InitWindow()
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

void GameAF::Run()
{
    using std::chrono::duration;
    using std::chrono::nanoseconds;
    using std::chrono::steady_clock;

    // 初始化随机数种子
    gen.seed(std::random_device{}());

    // 计算每帧运行的时间
    nanoseconds frame_duration((int)1e9 / m_fps);
    // 记录当前时刻
    steady_clock::time_point last_tick = steady_clock::now();
    steady_clock::time_point start_tick;
    // 物理固定帧
    float fixed_timestep = 1.0f / 60.0f;  // 60hz
    float accumulate_time = 0.0f;         // 时间累积器

#ifdef GAMEAF_USE_EASYX
    BeginBatchDraw();
#else
#endif

    auto& input = InputManager::GetInstance();
    auto& rigidbody = RigidbodyManager::GetInstance();
    auto& collision = CollisionManager::GetInstance();
    auto& scene = SceneManager::GetInstance();

    while (!m_exit) {
        // 处理消息
        input.ProcessInput();

        // 计算当前帧经过时间
        start_tick = steady_clock::now();
        auto delta = duration<float>(start_tick - last_tick);
        m_delta_time = delta.count();
        last_tick = start_tick;

        // 物理更新
        accumulate_time += m_delta_time;
        while (accumulate_time >= fixed_timestep) {
            rigidbody.OnFixedUpdate(fixed_timestep);
            collision.ProcessCollide(fixed_timestep);  // 碰撞检测 + 物理修正
            accumulate_time -= fixed_timestep;
            // 场景资源物理更新
            scene.OnFixUpdate(fixed_timestep);
        }

        // 场景数据更新
        scene.OnUpdate(m_delta_time);
        // TODO: 全局游戏对象数据更新

#ifdef GAMEAF_USE_EASYX
        setbkcolor(RGB(0, 0, 0));
        cleardevice();
#else
#endif

        // 渲染更新
        scene.OnRender();
        // TODO: 全局游戏对象渲染更新

#ifdef GAMEAF_USE_EASYX
        FlushBatchDraw();
#else
#endif
        // 清除帧间消息缓存
        InputManager::GetInstance().ClearFrameInput();

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

void GameAF::SetScreenSize(float width, float height)
{
    m_screenWidth = width;
    m_screenHeight = height;
}

void GameAF::SetShowConsole(bool isShowConsole) { m_isShowConsole = isShowConsole; }

void GameAF::SetScreenName(const std::string& screenName) { m_screenName = screenName; };

void GameAF::SetFPS(int fps)
{
    m_fps = fps < GAMEAF_MIN_FPS ? GAMEAF_MIN_FPS : fps > GAMEAF_MAX_FPS ? GAMEAF_MAX_FPS : fps;
}

void GameAF::Exit() { m_exit = true; }
