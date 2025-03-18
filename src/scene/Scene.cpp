#include <GameAf.h>
#include <input/InputManager.h>
#include <scene/Scene.h>

#include <algorithm>
#include <common/Log.hpp>

namespace gameaf {

Scene::Scene()
{
    auto [s_w, s_h] = GameAf::GetScreenSize();
    // 初始化main摄像机
    AddCamera("main", std::make_shared<Camera>(Vector2({s_w * 1.0f, s_h * 1.0f})));
}
Scene::~Scene() = default;

// 比较函数 根据z-order进行升序排序, zorder越高渲染级别越高，越被后渲染
static bool CompareByZOrder(const Scene::GameObjectPtr& ptr1, const Scene::GameObjectPtr& ptr2)
{
    return ptr1->GetZOrder() < ptr2->GetZOrder();
}

void Scene::OnUpdate(float delta)
{
    if (m_gameObjects) {
        if (m_needRenderLayerUpdate) {
            std::sort(m_gameObjects->begin(), m_gameObjects->end(), CompareByZOrder);
            m_needRenderLayerUpdate = false;
        }

        for (auto& object : *m_gameObjects) {
            object->OnUpdate(delta);
        }

        InputManager::GetInstance().ProcessMouseEvent(*m_gameObjects);
    }

    for (auto& [_, camera] : m_cameras) {
        camera->OnUpdate(delta);
    }
}

void Scene::OnFixUpdate(float alpha)
{
    if (m_gameObjects) {
        for (auto& object : *m_gameObjects) {
            object->OnFixUpdate(alpha);
        }
    }

    for (auto& [_, camera] : m_cameras) {
        camera->OnFixUpdate(alpha);
    }
}

void Scene::OnRender()
{
    if (m_gameObjects == nullptr) return;

    for (auto& object : *m_gameObjects) {
        object->OnRender();
    }

    if (m_renderCamera) {
        for (auto& [_, camera] : m_cameras) {
            camera->OnDebugRender();
        }
    }
}

void Scene::AddGameObject(GameObjectPtr game_object)
{
    // game_object->OnEnter();
    if (!game_object->m_isAwake) {
        game_object->OnAwake();  // 加载时调用一次
        game_object->m_isAwake = true;
    }

    // 设置myScene, 方便游戏对象在设置某种状态时, 能通知上层级的场景
    game_object->m_myScene = shared_from_this();
    if (m_gameObjects == nullptr) {
        m_gameObjects = std::make_unique<std::vector<GameObjectPtr>>();
    }
    // 添加场景内匹配的摄像机对象
    for (auto& [_, camera] : m_cameras) {
        if (camera->HasRender(game_object->GetName())) {
            game_object->AttachCamera(camera.get());
        }
    }

    m_gameObjects->push_back(std::move(game_object));
    m_needRenderLayerUpdate = true;  // 渲染的时候需要进行排序了
}

void Scene::AddGameObjects(const std::vector<GameObjectPtr>& game_objects)
{
    for (const auto& object : game_objects) {
        AddGameObject(std::move(object));
    }
}

void Scene::DelGameObjects(const std::string& id)
{
    if (m_gameObjects == nullptr) return;
    auto& input = InputManager::GetInstance();
    // 找到所有对应id的游戏对象，进行删除. 并且将摄像机追随的相关游戏对象也要进行删除
    auto it = m_gameObjects->begin();
    while (it != m_gameObjects->end()) {
        if ((*it)->GetName() == id) {
            input.ClearSenseCache((*it).get());
            it = m_gameObjects->erase(it);
        } else {
            ++it;
        }
    }
}
void Scene::DelAllGameObject()
{
    InputManager::GetInstance().ClearSenseCache();
    m_gameObjects = nullptr;
}

std::vector<Scene::GameObjectPtr> Scene::GetGameObjects(const std::string& id)
{
    if (!m_gameObjects) return {};
    std::vector<GameObjectPtr> game_objects;
    for (auto& object : (*m_gameObjects)) {
        if (object->GetName() == id) {
            game_objects.emplace_back(object);
        }
    }

    return game_objects;
}

Scene::GameObjectPtr Scene::GetGameObject(const std::string& id)
{
    if (!m_gameObjects) return nullptr;
    for (auto& object : (*m_gameObjects)) {
        if (object->GetName() == id) {
            return object;
        }
    }

    return nullptr;
}

bool Scene::AddCamera(const std::string& camera_id, CameraPtr camera)
{
    if (m_cameras.count(camera_id) != 0) return false;
    m_cameras.emplace(camera_id, std::move(camera));
    return true;
}
void Scene::SetCenterAnchorPoint(const std::string& camera_id, GameObjectPtr game_object)
{
    if (m_cameras.count(camera_id) == 0) return;
    auto& camera = m_cameras.at(camera_id);
    game_object->SetPosition(camera->GetPosition() + camera->GetSize() / 2);
}
void Scene::DelCamera(const std::string& camera_id)
{
    if (m_cameras.count(camera_id) == 0) return;
    m_cameras.erase(camera_id);
}

Scene::CameraPtr Scene::GetCamera(const std::string& camera_id)
{
    if (m_cameras.count(camera_id) == 0) {
        gameaf::log("[error][GetCamera] 摄像机未能找到 id:{}", camera_id);
        return nullptr;
    }
    return m_cameras.at(camera_id);
}

}  // namespace gameaf