#include "SceneManager.h"

#include <GameAf.h>
#include <common/Macros.h>
#include <input/InputManager.h>
#include <scene/Scene.h>

#include <common/Log.hpp>
#include <common/Utils.hpp>
#include <thread>

namespace gameaf {

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
    return m_scenePool.at(sceneId);
}

void SceneManager::OnUpdate(float delta)
{
    m_currentScene->OnUpdate();
    m_currentScene->OnUpdate(delta);

    // 过渡进度更新逻辑
    if (m_isTransition) {
        m_transitionProgress += m_transitionSpeed;
        if (m_transitionProgress >= 1.0f && m_swicthToSceneId != "") {
            m_currentScene = m_scenePool.at(m_swicthToSceneId);
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
    } else {
        m_currentScene = m_scenePool.at(sceneId);
        m_currentScene->OnEnter();
        if (isTransition) {
            m_isTransition = true;
            m_transitionProgress = 1.0f;
            m_transitionSpeed *= -1.0f;
            m_swicthToSceneId = "";
        }
    }
}
void SceneManager::SwitchTo(const std::string& sceneId, bool isTransition)
{
    if (m_scenePool.count(sceneId) == 0) {
        gameaf::log("[error][SwitchTo] 场景并未注册 sceneId: {}", sceneId);
        return;
    }
    m_currentScene->OnExit();
    InputManager::GetInstance().ClearSenseCache();  // 清理input中上个场景的缓存
    if (!isTransition) {
        m_currentScene = m_scenePool.at(sceneId);
        m_currentScene->OnEnter();
    } else {
        m_isTransition = true;  // 开始执行过渡逻辑
        m_swicthToSceneId = sceneId;
    }
}
void SceneManager::Register(const std::string& sceneId, SceneManager::ScenePtr scene)
{
    scene->OnAwake();
    m_scenePool[sceneId] = std::move(scene);
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
        SwitchTo(nextSceneId, isTransition);
    }).detach();
}

}  // namespace gameaf