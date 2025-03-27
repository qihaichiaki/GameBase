#pragma once

#include <common/StateMachine.hpp>
#include <functional>

namespace gameaf {

class Scene;

class SceneManager
{
public:
    using ScenePtr = std::shared_ptr<Scene>;
    using LoadAsyncFuc = std::function<void(SceneManager&)>;  // 异步加载函数回调

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
    /// @param nextSceneId 加载完毕的下一个场景1
    /// @param isTransition 是否过渡
    /// @note 如果没设置默认场景, loadSceneId就是第一个场景
    /// @warning loadFunc异步加载内容不能影响主线程内容(Gameobject创建等), 音频资源也不能异步加载(MCI问题)
    /// @warning 建议加载资源时使用异步, 除开字体和音频, 图集之类密集IO类型类型可以异步加载
    void LoadSceneAsync(LoadAsyncFuc loadFunc, const std::string& loadSceneId,
                        const std::string& nextSceneId, bool isTransition = false);

    /// @brief 获取全局的加载进度
    /// @note 通常使用在异步加载任务过程中, 异步任务共享进度给加载场景
    float GetLoadProgress();

    /// @brief 设置全局的加载进度
    /// @param progress 通常使用在异步加载任务中, 异步加载任务设置任务进度
    void SetLoadProgress(float progress);

    /// @brief 屏幕颜色渐变效果
    /// @param dstColorFactor 目标比例
    /// @param progress 进度
    /// @note 内容主要是[当前, 纯] 之间进行变换
    void ApplyFadeEffect(float dstColorFactor, float progress);

private:
    struct SceneInfo
    {
        ScenePtr scene = nullptr;
        bool isAwake = false;
    };

    ScenePtr m_currentScene = nullptr;
    std::unordered_map<std::string, SceneInfo> m_scenePool;

    // 场景过渡相关
    std::string m_swicthToSceneId;      // 待切换场景的id
    float m_dstColorFactor = 0.0f;      // 过渡中最终的目标大小 - 默认纯黑色(纯白有bug, 先屏蔽了)
    float m_transitionSpeed = 0.02f;    // 过渡速度
    float m_transitionProgress = 0.0f;  // 过渡进度
    bool m_isTransition = false;        // 是否开始过渡

private:
    SceneManager();
    ~SceneManager();
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
};
}  // namespace gameaf