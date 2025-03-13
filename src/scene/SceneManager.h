#pragma once

#include <common/StateMachine.hpp>

namespace gameaf {

class Scene;

class SceneManager
{
public:
    using ScenePtr = std::shared_ptr<Scene>;

    static SceneManager& GetInstance();
    /// @brief 获取对应id场景
    ScenePtr GetScene(const std::string& id);

public:
    // 游戏框架使用
    void OnFixUpdate(float alpha);
    void OnUpdate(float delta);
    void OnRender();

public:
    // 场景管理
    // 设置初始场景
    void SetEntry(const std::string& sceneId);
    // 切换场景
    void SwitchTo(const std::string& sceneId);
    // 注册场景
    void Register(const std::string& sceneId, ScenePtr scene);

private:
    ScenePtr m_currentScene = nullptr;
    std::unordered_map<std::string, ScenePtr> m_scenePool;

private:
    SceneManager() = default;
    ~SceneManager() = default;
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
};
}  // namespace gameaf