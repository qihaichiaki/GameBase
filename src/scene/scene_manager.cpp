#include "../include/scene_manager.h"

#include "../include/scene.h"

namespace gameaf {

SceneManager& SceneManager::getInstance()
{
    static SceneManager instance;
    return instance;
}

SceneManager::ScenePtr SceneManager::getScene(const std::string& id)
{
    return std::static_pointer_cast<Scene>(m_statePool.at(id));
}

void SceneManager::onUpdate(float delta)
{
    m_currentState->onUpdate();
    std::static_pointer_cast<Scene>(m_currentState)->onUpdate(delta);
}

void SceneManager::onRender() { std::static_pointer_cast<Scene>(m_currentState)->onRender(); }

}  // namespace gameaf