#pragma once

#include <game_object/component/Animation.h>

#include <string>
#include <unordered_map>

/**
 * 动画管理类
 * 负责管理多种帧动画, 存放在一起方便切换, 以及统一设置管理
 */

namespace gameaf {

struct Rect;
class Camera;
class Animator
{
public:
    Animator();
    ~Animator();

public:
    void OnUpdate(float delta);
    void OnRender(const Camera& camera, const Rect& rect);

public:
    /// @brief 添加动画对象
    /// @param animation_id 动画对象在当前动画管理器中的唯一id
    /// @param img 图像对象
    /// @param isLoop 是否循环
    /// @param interval 帧之间的间隔
    /// @return 是否添加成功
    bool AddAnimation(const std::string& animation_id, Image* img, bool isLoop = true,
                      float interval = 0.15f);

    /// @brief 添加动画对象
    /// @param animation_id 动画对象在当前动画管理器中的唯一id
    /// @param atlas 图集对象
    /// @param isLoop 是否循环
    /// @param interval 帧之间的间隔
    /// @return 是否添加成功
    bool AddAnimation(const std::string& animation_id, Atlas* atlas, bool isLoop = true,
                      float interval = 0.15f);

    /// @brief 删除动画对象
    /// @param animation_id 动画对象id
    /// @return 是否删除成功
    bool DelAnimation(const std::string& animation_id);

    /// @brief 获取动画对象
    /// @param animation_id 动画对象id
    /// @return 动画对象
    Animation& GetAnimation(const std::string& animation_id);

    /// @brief 获取动画对象
    /// @param animation_id 动画对象id
    /// @return 动画对象
    const Animation& GetAnimation(const std::string& animation_id) const;

    /// @brief 获取当前的动画对象
    Animation& GetCurrentAnimation();

    /// @brief 设置默认的动画对象
    /// @note 默认情况下如果没调用此函数, 第一个加入进来的动画对象
    void SetInitialAnimation(const std::string& initial_animation_id);

    /// @brief 获取默认动画对象
    /// @return 默认动画对象
    const Animation& GetInitialAnimation() const;

    /// @brief 动画管理器切换动画
    /// @param animation_id 切换目标动画
    /// @return 切换是否成功
    bool SwitchToAnimation(const std::string& animation_id);

private:
    std::unordered_map<std::string, Animation> m_animations;
    std::string m_current_animation_id;
    std::string m_initial_animation_id;
};
}  // namespace gameaf