#pragma once

#include "state_machine.hpp"

namespace gameaf {

class Scene;

class SceneManager : public StateMachine
{
public:
    using ScenePtr = std::shared_ptr<Scene>;

    static SceneManager& getInstance();
    /// @brief 获取对应id场景
    ScenePtr getScene(const std::string& id);

public:
    void onUpdate(float delta);
    void onRender();

private:
    SceneManager() = default;
    ~SceneManager() = default;
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
};
}  // namespace gameaf