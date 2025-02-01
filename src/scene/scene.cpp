#include "../include/scene.h"

#include <algorithm>

#include "../include/game_af.h"
#include "camera.hpp"

namespace gameaf {

Scene::Scene()
{
    // 初始化main摄像机
    newCamera();
}
Scene::~Scene() = default;

// 比较函数 根据z-order进行升序排序, zorder越高渲染级别越高，越被后渲染
static bool CompareByZOrder(const Scene::GameObjectPtr& ptr1, const Scene::GameObjectPtr& ptr2)
{
    return ptr1->getZOrder() < ptr2->getZOrder();
}

void Scene::onUpdate(float delta)
{
    if (m_gameObjects) {
        for (auto& object : *m_gameObjects) {
            object->onUpdate(delta);
            object->onUpdate();
        }
    }

    // 场景中其他更新
    // 摄像机跟随
    if (m_focusCameras) {
        for (auto& item : *m_focusCameras) {
            m_cameras.at(item.first).lookAt(item.second->getPosition());
        }
    }
}
void Scene::onRender()
{
    if (m_gameObjects == nullptr) return;
    if (m_needRenderLayerUpdate) {
        std::sort(m_gameObjects->begin(), m_gameObjects->end(), CompareByZOrder);
        m_needRenderLayerUpdate = false;
    }
    // TODO: camera 是否需要优化?
    for (const auto& camera : m_cameras) {
        for (auto& object : *m_gameObjects) {
            object->onRender(camera);
        }
    }
}

void Scene::addGameObject(GameObjectPtr game_object)
{
    game_object->onEnter();
    // 设置myScene, 方便游戏对象在设置某种状态时, 能通知上层级的场景
    game_object->m_myScene = shared_from_this();
    if (m_gameObjects == nullptr) {
        m_gameObjects = std::make_unique<std::vector<GameObjectPtr>>();
    }
    m_gameObjects->push_back(std::move(game_object));
    m_needRenderLayerUpdate = true;  // 渲染的时候需要进行排序了
}

void Scene::addGameObjects(const std::vector<GameObjectPtr>& game_objects)
{
    if (m_gameObjects == nullptr) {
        m_gameObjects = std::make_unique<std::vector<GameObjectPtr>>();
    }

    for (const auto& object : game_objects) {
        object->onEnter();
        object->m_myScene = shared_from_this();
        m_gameObjects->push_back(std::move(object));
    }
    m_needRenderLayerUpdate = true;  // 渲染的时候需要进行排序了
}

void Scene::delGameObjects(const std::string& id)
{
    if (m_gameObjects == nullptr) return;
    // 找到所有对应id的游戏对象，进行删除. 并且将摄像机追随的相关游戏对象也要进行删除
    auto it = m_gameObjects->begin();
    while (it != m_gameObjects->end()) {
        if ((*it)->getName() == id) {
            it = m_gameObjects->erase(it);

            // 删除内存中对应游戏对象追随摄像机信息
            if (m_focusCameras) {
                auto it_ = m_focusCameras->begin();
                while (it_ != m_focusCameras->end()) {
                    if (it_->second->getName() == id) {
                        it_ = m_focusCameras->erase(it_);
                    } else {
                        ++it_;
                    }
                }
            }
        } else {
            ++it;
        }
    }
}
void Scene::delAllGameObject()
{
    m_gameObjects = nullptr;
    m_focusCameras = nullptr;
}

std::vector<Scene::GameObjectPtr> Scene::getGameObjects(const std::string& id)
{
    if (m_gameObjects) return {};
    std::vector<GameObjectPtr> game_objects;
    for (auto& object : (*m_gameObjects)) {
        if (object->getName() == id) {
            game_objects.emplace_back(object);
        }
    }

    return game_objects;
}

size_t Scene::newCamera()
{
    auto [screenW, screenH] = GameAF::getInstance().getScreenSize();
    m_cameras.emplace_back(Camera({(float)screenW, (float)screenH}));
    return m_cameras.size() - 1;
}
bool Scene::setCameraSize(size_t id, const Vector2& size)
{
    if (m_cameras.size() <= id) return false;
    m_cameras.at(id).setSize(size);
    return true;
}
bool Scene::setCameraPos(size_t id, const Vector2& pos)
{
    if (m_cameras.size() <= id) return false;
    m_cameras.at(id).setPosition(pos);
    return true;
}
void Scene::CameraFocusOn(size_t id, GameObjectPtr game_object)
{
    if (m_focusCameras == nullptr) {
        m_focusCameras = std::make_unique<std::unordered_map<size_t, GameObjectWeakPtr>>();
    }
    m_focusCameras->emplace(id, game_object.get());
}

void Scene::setCenterAnchorPoint(size_t id, GameObjectPtr game_object)
{
    if (m_cameras.size() <= id) return;
    auto& camera = m_cameras.at(id);
    game_object->setPosition(camera.getPosition() + camera.getSize() / 2);
}

// TODO: 删除摄像机后，删除的后面的摄像机的id会发生调整
void Scene::delCamera(size_t id)
{
    if (m_cameras.size() <= id) return;
    m_cameras.erase(m_cameras.begin() + id);
}

}  // namespace gameaf