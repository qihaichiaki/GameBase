#include "GameAf.h"

#include <common/Macros.h>
#include <input/InputManager.h>
#include <scene/SceneManager.h>

#include <chrono>
#include <codecvt>
#include <common/Log.hpp>
#include <common/Utils.hpp>
#include <game_object/component/CollisionManager.hpp>
#include <thread>

using namespace gameaf;

static int g_screenWidth;
static int g_screenHeight;

GameAf& GameAf::GetInstance()
{
    static GameAf instance;
    return instance;
}

void GameAf::InitWindow(const std::string& iconPath, bool isShowConsole,
                        const std::string& screenName, int screenWidth, int screenHeight)
{
    // 初始化屏幕
    WINDOWS_USE_UTF8;

    // 实例化单例, 控制单例正确的顺序
    (void)InputManager::GetInstance();
    (void)CollisionManager::GetInstance();
    (void)SceneManager::GetInstance();

#ifdef GAMEAF_USE_EASYX
    HWND hwnd;
    if (isShowConsole) {
        hwnd = initgraph(screenWidth, screenHeight, EX_SHOWCONSOLE);
    } else {
        hwnd = initgraph(screenWidth, screenHeight);
        FreeConsole();  // 隐藏控制台
    }

    SetWindowTextW(hwnd, UTF8StrToWStr(screenName).c_str());
    g_screenWidth = screenWidth;
    g_screenHeight = screenHeight;

    if (iconPath == "") return;
    // 从文件加载图标
    HICON hIcon =
        (HICON)LoadImageW(NULL, UTF8StrToWStr(iconPath).c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    if (hIcon) {
        // 设置窗口的大图标（任务栏、窗口标题）
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        // 设置窗口的小图标（任务栏缩略图、Alt+Tab 切换）
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    } else {
        DWORD err = GetLastError();
        gameaf::log("[warring][InitWindow] path: {} error: {}", iconPath, err);
    }
#else
#endif
}

void GameAf::Run()
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
    float accumulate_time = 0.0f;  // 时间累积器

#ifdef GAMEAF_USE_EASYX
    BeginBatchDraw();
#else
#endif

    auto& input = InputManager::GetInstance();
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
        while (accumulate_time >= m_fixed_timestep) {
            // 场景资源物理更新
            scene.OnFixUpdate(m_fixed_timestep);
            collision.ProcessCollide(m_fixed_timestep);  // 碰撞检测 + 物理修正
            accumulate_time -= m_fixed_timestep;
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
        input.ClearFrameInput();
        // 清除碰撞状态
        collision.ClearCollided();

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
std::tuple<int, int> GameAf::GetScreenSize() { return {g_screenWidth, g_screenHeight}; }

int GameAf::GetScreenWidth() { return g_screenWidth; }

int GameAf::GetScreenHeight() { return g_screenHeight; }

void GameAf::SetFPS(int fps)
{
    m_fps = fps < GAMEAF_MIN_FPS ? GAMEAF_MIN_FPS : fps > GAMEAF_MAX_FPS ? GAMEAF_MAX_FPS : fps;
}

void GameAf::Exit() { m_exit = true; }
