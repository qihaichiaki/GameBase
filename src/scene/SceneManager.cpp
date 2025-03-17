#include "SceneManager.h"

#include <GameAf.h>
#include <common/Macros.h>
#include <input/InputManager.h>
#include <scene/Scene.h>

#include <atomic>
#include <common/Log.hpp>
#include <common/Utils.hpp>
#include <mutex>
#include <queue>
#include <thread>

namespace gameaf {

// 就场景管理器全局的主线程任务队列
// 用于异步任务再主线程执行逻辑
static std::queue<std::function<void()>> mainThreadQueue;
static std::mutex queueMutex;
static std::atomic<float> loadingProgress = 0.0f;  // 共享进度变量

inline static void RunOnMainThread(std::function<void()> func)
{
    std::lock_guard<std::mutex> lock(queueMutex);
    mainThreadQueue.push(func);
}

SceneManager& SceneManager::GetInstance()
{
    static SceneManager instance;
    return instance;
}

SceneManager::ScenePtr SceneManager::GetScene(const std::string& sceneId)
{
    if (m_scenePool.count(sceneId) == 0) {
        gameaf::log("[error][GetScene] 场景并未注册 sceneId: {}", sceneId);
        return nullptr;
    }
    return m_scenePool.at(sceneId).scene;
}

void SceneManager::OnUpdate(float delta)
{
    m_currentScene->OnUpdate();
    m_currentScene->OnUpdate(delta);

    // 过渡进度更新逻辑
    if (m_isTransition) {
        m_transitionProgress += m_transitionSpeed;
        if (m_transitionProgress >= 1.0f && m_swicthToSceneId != "") {
            m_currentScene->OnExit();  // 上个场景退出逻辑
            m_currentScene = m_scenePool.at(m_swicthToSceneId).scene;
            m_currentScene->OnEnter();
            m_transitionProgress = 1.0f;
            m_transitionSpeed *= -1.0f;
            m_swicthToSceneId = "";
        } else if (m_transitionProgress <= 0.0f) {
            m_isTransition = false;
            m_transitionSpeed *= -1.0f;
            m_transitionProgress = 0.0f;
        }
    }

    // 主线程任务队列
    // 没有锁的保护, 可能会出现问题
    while (!mainThreadQueue.empty()) {
        mainThreadQueue.front()();
        mainThreadQueue.pop();
    }
}

void SceneManager::OnFixUpdate(float alpha) { m_currentScene->OnFixUpdate(alpha); }

/// @brief 屏幕颜色渐变效果
/// @note 内容主要是[当前, 纯] 之间进行变换
static void ApplyFadeEffect(float dstColorFactor, float progress)
{
#ifdef GAMEAF_USE_EASYX
    DWORD* buffer = GetImageBuffer();  // 获取当前屏幕图像缓冲
    if (buffer == nullptr) return;
    int w = getwidth(), h = getheight();
    // 宽度和高度
    float factor = Lerp(1.0f, dstColorFactor, progress);  // 预计算渐变因子
    constexpr DWORD alpha_mask = 0xFF000000;              // 预定义 Alpha 通道

    for (int y = 0; y < h; y++) {
        DWORD* row_ptr = buffer + y * w;  // 逐行处理，提高缓存命中率
        for (int x = 0; x < w; x++, row_ptr++) {
            DWORD color = *row_ptr;  // 降低 CPU 内存访问,.让其尽量放在寄存器?
            BYTE r = (BYTE)(GetBValue(color) * factor);
            BYTE g = (BYTE)(GetGValue(color) * factor);
            BYTE b = (BYTE)(GetRValue(color) * factor);
            *row_ptr = BGR(RGB(r, g, b)) | alpha_mask;
        }
    }
#else
#endif
}

void SceneManager::OnRender()
{
    m_currentScene->OnRender();

    if (m_isTransition) {
        ApplyFadeEffect(m_dstColorFactor, m_transitionProgress);
    }
}

void SceneManager::SetEntry(const std::string& sceneId, bool isTransition)
{
    if (m_scenePool.count(sceneId) == 0) {
        gameaf::log("[error][SetEntry] 场景并未注册 sceneId: {}", sceneId);
        return;
    }

    if (m_currentScene != nullptr) {
        gameaf::log("[error][SetEntry] 当前并不是初始阶段, 已经存在场景运行");
        return;
    }

    auto& sceneInfo = m_scenePool.at(sceneId);
    m_currentScene = sceneInfo.scene;
    if (sceneInfo.isAwake) {
        // 不应该出现, 这里做个简单检查
        gameaf::log("[error][SetEntry] {}初始场景被调用过OnAwake方法了.....", sceneId);
    } else {
        m_currentScene->OnAwake();  // 初始阶段调用一次
        sceneInfo.isAwake = true;
    }

    m_currentScene->OnEnter();
    if (isTransition) {
        m_isTransition = true;
        m_transitionProgress = 1.0f;
        m_transitionSpeed *= -1.0f;
        m_swicthToSceneId = "";
    }
}
void SceneManager::SwitchTo(const std::string& sceneId, bool isTransition)
{
    if (m_scenePool.count(sceneId) == 0) {
        gameaf::log("[error][SwitchTo] 场景并未注册 sceneId: {}", sceneId);
        return;
    }
    InputManager::GetInstance().ClearSenseCache();  // 清理input中上个场景的缓存
    auto& sceneInfo = m_scenePool.at(sceneId);
    if (!sceneInfo.isAwake) {
        sceneInfo.scene->OnAwake();  // 在实际enter前调用OnAwake
        sceneInfo.isAwake = true;
    }
    if (!isTransition) {
        m_currentScene->OnExit();
        m_currentScene = sceneInfo.scene;
        m_currentScene->OnEnter();
    } else {
        m_isTransition = true;  // 开始执行过渡逻辑
        m_swicthToSceneId = sceneId;
    }
}
void SceneManager::Register(const std::string& sceneId, SceneManager::ScenePtr scene)
{
    m_scenePool[sceneId].scene = std::move(scene);
}

void SceneManager::LoadSceneAsync(LoadAsyncFuc loadFunc, const std::string& loadSceneId,
                                  const std::string& nextSceneId, bool isTransition)
{
    if (m_scenePool.count(loadSceneId) == 0 || m_scenePool.count(nextSceneId) == 0) {
        gameaf::log("[error][LoadSceneAsync] 存在场景并未注册");
        return;
    }

    if (m_currentScene)
        SwitchTo(loadSceneId, isTransition);
    else {
        SetEntry(loadSceneId, isTransition);
    }

    // 开始异步加载
    std::thread([=]() {
        loadFunc();
        // 执行完毕, 在主线程切换场景
        RunOnMainThread([=]() { SwitchTo(nextSceneId, isTransition); });
    }).detach();
}

float SceneManager::GetLoadProgress() { return loadingProgress; }

void SceneManager::SetLoadProgress(float progress) { loadingProgress = progress; }

SceneManager::SceneManager() {}
SceneManager::~SceneManager() {}

}  // namespace gameaf