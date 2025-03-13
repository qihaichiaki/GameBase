#include "SceneManager.h"

#include <scene/Scene.h>

#include <common/Log.hpp>

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
}

void SceneManager::OnFixUpdate(float alpha) { m_currentScene->OnFixUpdate(alpha); }

void SceneManager::OnRender() { m_currentScene->OnRender(); }

void SceneManager::SetEntry(const std::string& sceneId)
{
    if (m_scenePool.count(sceneId) == 0) {
        gameaf::log("[error][SetEntry] 场景并未注册 sceneId: {}", sceneId);
    } else {
        m_currentScene = m_scenePool.at(sceneId);
    }
}
void SceneManager::SwitchTo(const std::string& sceneId)
{
    m_currentScene->OnExit();
    m_currentScene = m_scenePool.at(sceneId);
    m_currentScene->OnEnter();
}
void SceneManager::Register(const std::string& sceneId, SceneManager::ScenePtr scene)
{
    scene->OnAwake();
    m_scenePool[sceneId] = std::move(scene);
    if (m_currentScene == nullptr) m_currentScene = m_scenePool[sceneId];
}

}  // namespace gameaf