#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "game_object.h"
#include "state_node.hpp"

namespace gameaf {

class Camera;

class Scene : public StateNode, public std::enable_shared_from_this<Scene>
{
public:
    using GameObjectPtr = ::std::shared_ptr<GameObject>;
    using GameObjects = ::std::unique_ptr<std::vector<GameObjectPtr>>;
    using Cameras = std::vector<Camera>;
    using GameObjectWeakPtr = GameObject*;
    using FocusCameras = std::unique_ptr<std::unordered_map<size_t, GameObjectWeakPtr>>;

public:
    Scene();
    ~Scene();

public:
    void onUpdate(float delta);
    void onRender();

public:
    /// @brief 添加游戏对象
    void addGameObject(GameObjectPtr game_object);
    /// @brief 批量添加游戏对象
    void addGameObjects(const std::vector<GameObjectPtr>& game_objects);
    /// @brief 删除对应id的游戏对象(们)
    void delGameObjects(const std::string& id);
    /// @brief 删除全部的游戏对象
    void delAllGameObject();
    /// @brief 获取当前场景下对应名字的游戏对象
    std::vector<GameObjectPtr> getGameObjects(const std::string& id);
    /// @brief 游戏对象渲染层级更新
    void notifyRenderLayerUpdate() { m_needRenderLayerUpdate = true; }
    /// @brief 添加场景摄像机, 返回新增场景摄像机编号
    /// @note 新增摄像机后, 默认位置为世界坐标(0, 0), 大小和游戏屏幕大小一致
    size_t newCamera();
    /// @brief 设置摄像机大小
    /// @param id 摄像机id
    /// @param size 摄像机大小
    /// @return 是否设置成功(摄像机id是否正确)
    bool setCameraSize(size_t id, const Vector2& size);
    /// @brief 设置摄像机位置
    /// @param id 摄像机id
    /// @param pos 摄像机世界坐标(矩形的左上角)
    /// @return 是否设置成功(摄像机id是否正确)
    bool setCameraPos(size_t id, const Vector2& pos);
    /// @brief 指定摄像机聚焦游戏对象
    /// @param id 摄像机id
    /// @param game_object 游戏对象
    void CameraFocusOn(size_t id, GameObjectPtr game_object);
    /// @brief 设置游戏对象的锚点位于对应摄像机的中心位置(设置一次, 不会追随)
    /// @param game_object
    void setCenterAnchorPoint(size_t id, GameObjectPtr game_object);
    /// @brief 删除摄像机对象
    void delCamera(size_t id);

private:
    // 场景下的所有游戏对象
    GameObjects m_gameObjects = nullptr;
    // 场景下的所有摄像机对象, 默认存在一个
    Cameras m_cameras;
    // 场景下的摄像机跟随目标
    FocusCameras m_focusCameras = nullptr;

    // 是否需要排序 在渲染对象的时候, 判断当前是否需要排序
    bool m_needRenderLayerUpdate = false;
};
}  // namespace gameaf