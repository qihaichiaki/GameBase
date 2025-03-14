#pragma once

#include <game_object/component/Animation.h>
#include <game_object/component/Component.h>

#include <string>
#include <unordered_map>

/**
 * 动画管理类
 * 负责管理多种帧动画, 存放在一起方便切换, 以及统一设置管理
 */

namespace gameaf {

class Camera;
class Atlas;
class Animator : public Component
{
public:
    Animator(GameObject* obj);
    ~Animator();

public:
    void OnUpdate(float delta);
    void OnRender(const Camera& camera);
    void SetGameObject(GameObject* obj) override;
    void Flip() override;

public:
    /// @brief 添加动画对象来自图像
    /// @param animation_id 动画对象在当前动画管理器中的唯一id
    /// @param imgID 图元对象ID
    /// @param isLoop 是否循环
    /// @param interval 帧之间的间隔
    /// @return 是否添加成功
    bool AddAnimationForImage(const std::string& animation_id, const std::string& imgID,
                              bool isLoop = true, float interval = 0.15f);

    /// @brief 添加动画对象来自图集
    /// @param animation_id 动画对象在当前动画管理器中的唯一id
    /// @param atlasID 图集对象ID
    /// @param isLoop 是否循环
    /// @param interval 帧之间的间隔
    /// @return 是否添加成功
    bool AddAnimationForAtlas(const std::string& animation_id, const std::string& atlasID,
                              bool isLoop = true, float interval = 0.15f);

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

    bool ContainsScreenPoint(const Camera&, const Vector2&) const override;

    /// @brief 动画全部重置
    void Restart();

private:
    std::unordered_map<std::string, Animation> m_animations;
    std::string m_current_animation_id;
    std::string m_initial_animation_id;
};
}  // namespace gameaf