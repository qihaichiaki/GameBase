#include "SceneManager.h"

#include <scene/Scene.h>

namespace gameaf {

SceneManager& SceneManager::GetInstance()
{
    static SceneManager instance;
    return instance;
}

SceneManager::ScenePtr SceneManager::GetScene(const std::string& id)
{
    return std::static_pointer_cast<Scene>(m_statePool.at(id));
}

void SceneManager::OnUpdate(float delta)
{
    m_currentState->onUpdate();
    std::static_pointer_cast<Scene>(m_currentState)->OnUpdate(delta);
}

void SceneManager::OnFixUpdate(float alpha)
{
    std::static_pointer_cast<Scene>(m_currentState)->OnFixUpdate(alpha);
}

void SceneManager::OnRender() { std::static_pointer_cast<Scene>(m_currentState)->OnRender(); }

}  // namespace gameaf