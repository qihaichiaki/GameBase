#pragma once

#include <common/Vector2.hpp>
#include <game_object/component/Timer.hpp>
#include <unordered_set>

// TODO: 摄像机对象放出来, 不单独为场景所拥有
// TODO: 做成游戏对象进行加载 ?

namespace gameaf {

class GameObject;
class Rigidbody2D;
class Camera
{
public:
    enum class FollowMode { None, Smooth };

public:
    Camera();
    Camera(Vector2 size);
    ~Camera();

public:
    /// @brief 获取相机的左上角位置
    const Vector2& GetPosition() const;

    /// @brief 设置相机的左上角位置
    void SetPosition(Vector2 position) { m_position = position; }

    /// @brief 获取相机的大小
    const Vector2& GetSize() const { return m_size; }

    /// @brief 设置相机的大小
    void SetSize(Vector2 size) { m_size = size; }

    /// @brief 指定添加相机渲染的游戏对象
    /// @warning 只在非全局渲染模式下可用
    void AddRenderObj(const std::string& obj) { m_render_objs.insert(obj); }

    /// @brief 指定不渲染的游戏对象
    /// @warning 只在全局渲染模式下可用, 指定游戏对象渲染不可用
    void DisableRenderObj(const std::string& obj) { m_unrender_objs.insert(obj); }

    /// @brief 删除要渲染的游戏对象
    void DelRenderObj(const std::string& obj) { m_render_objs.erase(obj); }

    /// @brief 设置渲染模式
    /// @param globalRender 全局渲染模式?
    void SetRenderMod(bool globalRender) { m_isGlobalRender = globalRender; }

    /// @brief 当前相机是否渲染对应的游戏对象
    /// @param obj 游戏对象名字
    bool HasRender(const std::string& obj)
    {
        if (m_isGlobalRender) {
            return !m_unrender_objs.count(obj);
        } else {
            return m_render_objs.count(obj);
        }
    }

    /// @brief 清除渲染对象缓存
    void ClearRender()
    {
        m_render_objs.clear();
        m_unrender_objs.clear();
    }

    /// @brief 摄像机锁定中心位置
    void LookAt(Vector2 position) { m_position = position - m_size / 2.0f; }

    /// @brief 设置追随中心对象
    void SetFollowTarget(std::shared_ptr<GameObject> target_obj,
                         FollowMode mode = FollowMode::None);

    /// @brief 设置相机的死区, 让相机跟随的目标如果在这些区域时, 相机不会跟随移动
    /// @param size 死区的大小
    /// @note 默认偏移位置位于相机大小的正中心
    void SetCameraDeadZone(const Vector2& size);

    /// @brief 设置相机的死区, 让相机跟随的目标如果在这些区域时, 相机不会跟随移动
    /// @param offset 死区相较于相机左上角的位移
    /// @param size 死区的大小
    void SetCameraDeadZone(const Vector2& offset, const Vector2& size);

    /// @brief 相机震动
    /// @param duration 持续时间
    /// @param shakeIntensity 震动强度
    void Shake(float duration, float shakeIntensity = 19.8);

    /// @brief 设置相机的固定死区, 相机不会在追随时向死区外移动
    /// @param fixdDeadZonePos 固定死区的世界坐标(中心点)
    /// @param fixdDeadZoneSize 固定死区大小
    void SetFixdDeadZone(const Vector2& fixdDeadZonePos, const Vector2& fixdDeadZoneSize);

public:
    /// @brief 相机物理更新
    /// @param alpha 物理频率更新
    void OnFixUpdate(float alpha);

    /// @brief 帧率更新
    void OnUpdate(float delta);

    /// @brief 相机辅助渲染
    void OnDebugRender();

private:
    /// @brief 正常模式跟随
    Vector2 __onNormalFollow(float alpha, const Vector2& current_position);

    /// @brief 死区模式跟随
    Vector2 __onDeadZoneFollow(float alpha, const Vector2& current_position);

private:
    Vector2 m_position;                               // 相机的左上角的世界坐标
    Vector2 m_size;                                   // 相机的大小(宽高)
    Vector2 m_target_position;                        // 目标位置
    GameObject* m_target_obj = nullptr;               // 目标对象
    Vector2 m_deadZoneOffset;                         // 死区偏移
    Vector2 m_deadZoneSize;                           // 死区大小
    float m_smooth_factor = 9.8f;                     // 平滑因子系数, 控制跟随效果
    FollowMode m_follow_mode;                         // 追随模式
    bool m_isGlobalRender = true;                     // 默认全局渲染模式
    std::unordered_set<std::string> m_render_objs;    // 指定渲染目标游戏对象
    std::unordered_set<std::string> m_unrender_objs;  // 指定非渲染游戏对象
    Timer m_shakeTimer;                               // shake 定时器, 用于控制是否shake
    Vector2 m_shakePosition;                          // 当前震动位置, 方便使用
    float m_shakeIntensity;                           // 震动强度
    bool is_shaking = false;                          // 控制是否执行shake行为
    // 固定死区, 即拥有世界坐标的死区, 摄像机不会往该死区外移动
    Vector2 m_fixedDeadZonePos;   // 固定死区位置
    Vector2 m_fixedDeadZoneSize;  // 固定死区大小
};

}  // namespace gameaf