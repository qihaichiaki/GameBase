#pragma once

#include <unordered_set>

#include "../include/game_object.h"
#include "../include/vector2.hpp"

// TODO: 摄像机对象放出来, 不单独为场景所拥有
// TODO: 做成游戏对象进行加载 ?

namespace gameaf {
class Camera
{
public:
    enum class FollowMode { None, Smooth };

public:
    Camera() = default;
    Camera(Vector2 size) : m_size(size) {}
    ~Camera() = default;

    Vector2 getPosition() const { return m_position; }
    void setPosition(Vector2 position) { m_position = position; }

    Vector2 getSize() const { return m_size; }
    void setSize(Vector2 size) { m_size = size; }

    void addRenderObj(const std::string& obj)
    {
        m_render_objs.insert(obj);
        if (is_all_render) is_all_render = false;
    }
    void delRenderObj(const std::string& obj) { m_render_objs.erase(obj); }
    void allRender()
    {
        is_all_render = true;
        m_render_objs.clear();
    }
    bool isAllRender() { return is_all_render; }
    bool hasRender(const std::string& obj) { return m_render_objs.count(obj); }

    /// @brief 摄像机锁定中心位置
    void lookAt(Vector2 position) { m_position = position - m_size / 2.0f; }

    /// @brief 设置追随中心对象
    void setFollowTarget(std::shared_ptr<GameObject> target_obj, FollowMode mode = FollowMode::None)
    {
        m_target_obj = target_obj;
        m_follow_mode = mode;
    }

public:
    void onUpdate(float delta);

private:
    Vector2 m_position;                             // 相机的左上角的世界坐标
    Vector2 m_size;                                 // 相机的大小(宽高)
    Vector2 m_target_position;                      // 目标位置
    std::weak_ptr<GameObject> m_target_obj;         // 目标对象
    Vector2 m_diff;                                 // 不一定是中心位置, 可能存在一定视察
    float m_smooth_factor = 0.1f;                   // 平滑因子系数, 控制跟随效果
    FollowMode m_follow_mode;                       // 追随模式
    bool is_all_render = true;                      // 渲染当前场景下的所有游戏对象
    std::unordered_set<std::string> m_render_objs;  // 渲染目标游戏对象
    // TODO: 摄像机之间的优先级?
};

}  // namespace gameaf