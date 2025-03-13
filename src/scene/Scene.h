#pragma once

#include <game_object/GameObject.h>
#include <scene/Camera.h>

#include <common/StateNode.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace gameaf {

class Camera;

class Scene : public StateNode, public std::enable_shared_from_this<Scene>
{
public:
    using GameObjectPtr = ::std::shared_ptr<GameObject>;
    using GameObjects = ::std::unique_ptr<std::vector<GameObjectPtr>>;
    using GameObjectWeakPtr = GameObject*;
    using CameraPtr = std::shared_ptr<Camera>;
    using Cameras = std::unordered_map<std::string, CameraPtr>;

public:
    Scene();
    ~Scene() override;

public:
    /// @brief 物理循环更新
    void OnFixUpdate(float alpha);

    /// @brief 帧率更新
    void OnUpdate(float delta);

    /// @brief 渲染
    void OnRender();

public:
    /// @brief 添加游戏对象
    void AddGameObject(GameObjectPtr game_object);
    /// @brief 批量添加游戏对象
    void AddGameObjects(const std::vector<GameObjectPtr>& game_objects);
    /// @brief 删除对应id的游戏对象(们)
    void DelGameObjects(const std::string& id);
    /// @brief 删除全部的游戏对象
    void DelAllGameObject();
    /// @brief 获取当前场景下对应名字的游戏对象
    std::vector<GameObjectPtr> GetGameObjects(const std::string& id);
    /// @brief 游戏对象渲染层级更新
    void NotifyRenderLayerUpdate() { m_needRenderLayerUpdate = true; }
    /// @brief 添加场景摄像机
    /// @note 新增摄像机后, 默认位置为世界坐标(0, 0), 大小和游戏屏幕大小一致
    bool AddCamera(const std::string& camera_id, CameraPtr camera);
    /// @brief 设置游戏对象的锚点位于对应摄像机的中心位置(设置一次, 不会追随)
    /// @param game_object
    void SetCenterAnchorPoint(const std::string& camera_id, GameObjectPtr game_object);
    /// @brief 删除摄像机对象
    void DelCamera(const std::string& camera_id);
    /// @brief 获取摄像机对象
    CameraPtr GetCamera(const std::string& camera_id)
    {
        if (m_cameras.count(camera_id) == 0) return nullptr;
        return m_cameras.at(camera_id);
    }
    /// @brief 设置是否debug渲染相机辅助区域
    void SetDebugRenderCamera(bool renderCamera) { m_renderCamera = renderCamera; }

private:
    // 场景下的所有游戏对象
    GameObjects m_gameObjects = nullptr;
    // 场景下的所有摄像机对象, 默认存在一个
    Cameras m_cameras;

    // 是否需要排序 在渲染对象的时候, 判断当前是否需要排序
    bool m_needRenderLayerUpdate = false;

    // 是否渲染相机辅助划线
    bool m_renderCamera = false;
};
}  // namespace gameaf