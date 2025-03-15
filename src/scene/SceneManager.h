#pragma once

#include <common/StateMachine.hpp>
#include <functional>

namespace gameaf {

class Scene;

class SceneManager
{
public:
    using ScenePtr = std::shared_ptr<Scene>;
    using LoadAsyncFuc = std::function<void()>;  // 异步加载函数回调

    static SceneManager& GetInstance();
    /// @brief 获取对应id场景
    ScenePtr GetScene(const std::string& id);

public:
    // 游戏框架使用
    void OnFixUpdate(float alpha);
    void OnUpdate(float delta);
    void OnRender();

public:
    /// @brief 设置初始场景
    /// @param sceneId 初始场景id
    /// @param isTransition 初始场景是否需要过渡
    void SetEntry(const std::string& sceneId, bool isTransition = false);

    /// @brief 切换场景
    /// @param sceneId 切换的目标场景
    /// @param isTransition 是否需要过渡
    void SwitchTo(const std::string& sceneId, bool isTransition = false);

    /// @brief 注册场景
    void Register(const std::string& sceneId, ScenePtr scene);

    /// @brief 异步加载场景
    /// @param loadFunc 异步加载内容
    /// @param loadSceneId 加载时显示场景
    /// @param nextSceneId 加载完毕的下一个场景
    /// @param isTransition 是否过渡
    /// @note 如果没设置默认场景, loadSceneId就是第一个场景
    void LoadSceneAsync(LoadAsyncFuc loadFunc, const std::string& loadSceneId,
                        const std::string& nextSceneId, bool isTransition = false);

private:
    ScenePtr m_currentScene = nullptr;
    std::unordered_map<std::string, ScenePtr> m_scenePool;

    // 场景过渡相关
    std::string m_swicthToSceneId;      // 待切换场景的id
    float m_dstColorFactor = 0.0f;      // 过渡中最终的目标大小 - 默认纯黑色(纯白有bug, 先屏蔽了)
    float m_transitionSpeed = 0.02f;    // 过渡速度
    float m_transitionProgress = 0.0f;  // 过渡进度
    bool m_isTransition = false;        // 是否开始过渡

private:
    SceneManager() = default;
    ~SceneManager() = default;
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
};
}  // namespace gameaf