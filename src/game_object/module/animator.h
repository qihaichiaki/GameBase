#pragma once

#include <string>
#include <unordered_map>

#include "animation.hpp"

/**
 * 动画管理类
 * 负责管理多种帧动画, 存放在一起方便切换, 以及统一设置管理
 */

namespace gameaf {

struct Rect;
class Camera;
class Animation;

class Animator
{
public:
    Animator();
    ~Animator();

public:
    void onUpdate(float delta);
    void onRender(const Camera&, const Rect&);

    Vector2 currentAnimationSize() const
    {
        return m_animations.at(m_current_animation_id).currentFrameSize();
    }

public:
    bool newAnimationForImage(const std::string& animation_id, const std::string& img_id,
                              float interval, bool is_loop);
    bool newAnimationForAtlas(const std::string& animation_id, const std::string& atlas_id,
                              float interval, bool is_loop);
    bool delAnimation(const std::string& animation_id);
    bool setInitialAnimation(const std::string& animation_id)
    {
        if (m_animations.count(animation_id) == 0) return false;
        m_initial_animation_id = animation_id;
        if (m_current_animation_id == "") m_current_animation_id = animation_id;
        return true;
    }
    bool switchToAnimation(const std::string& animation_id)
    {
        if (m_animations.count(animation_id) == 0) return false;
        // 变换处理?
        m_current_animation_id = animation_id;
        auto& animation = m_animations.at(animation_id);
        if (!animation.isLoop() && animation.isFrameLastIndex()) {
            animation.restart();
        }
        return true;
    }
    const Animation& findAnimation(const std::string& animation_id) const
    {
        return m_animations.at(animation_id);
    }
    const Animation& initialAnimation() const { return m_animations.at(m_initial_animation_id); }

private:
    std::unordered_map<std::string, Animation> m_animations;
    std::string m_current_animation_id;
    std::string m_initial_animation_id;
};
}  // namespace gameaf