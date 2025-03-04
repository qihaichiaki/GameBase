#pragma once

#include <common/StateMachine.hpp>

namespace gameaf {

class Scene;

class SceneManager : public StateMachine
{
public:
    using ScenePtr = std::shared_ptr<Scene>;

    static SceneManager& GetInstance();
    /// @brief 获取对应id场景
    ScenePtr GetScene(const std::string& id);

public:
    void OnUpdate(float delta);
    void OnRender();

private:
    SceneManager() = default;
    ~SceneManager() = default;
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
};
}  // namespace gameaf