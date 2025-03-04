#include "Animator.h"

#include <common/Common.h>
#include <resource/ResourceManager.h>

namespace gameaf {

Animator::Animator() = default;
Animator::~Animator() = default;

void Animator::OnUpdate(float delta) { m_animations.at(m_current_animation_id).OnUpdate(delta); }

void Animator::OnRender(const Camera& camera, const Rect& rect)
{
    m_animations.at(m_current_animation_id).OnRender(camera, rect);
}

bool Animator::AddAnimation(const std::string& animation_id, Image* img, bool isLoop,
                            float interval)
{
    if (m_animations.count(animation_id) != 0) return false;

    if (m_initial_animation_id == "") {
        m_initial_animation_id = animation_id;
    }
    m_animations[animation_id].AddFrame(img);
    m_animations[animation_id].SetLoop(isLoop);
    m_animations[animation_id].SetInterval(interval);
    return true;
}

bool Animator::AddAnimation(const std::string& animation_id, Atlas* atlas, bool isLoop,
                            float interval)
{
    if (m_animations.count(animation_id) != 0) return false;

    if (m_initial_animation_id == "") {
        m_initial_animation_id = animation_id;
    }
    m_animations[animation_id].AddFrame(atlas);
    m_animations[animation_id].SetLoop(isLoop);
    m_animations[animation_id].SetInterval(interval);
    return true;
}

bool Animator::DelAnimation(const std::string& animation_id)
{
    if (m_animations.count(animation_id) == 0) return false;

    m_animations.erase(animation_id);
    return true;
}

Animation& Animator::GetAnimation(const std::string& animation_id)
{
    return m_animations.at(animation_id);
}

const Animation& Animator::GetAnimation(const std::string& animation_id) const
{
    return m_animations.at(animation_id);
}

Animation& Animator::GetCurrentAnimation() { return m_animations.at(m_current_animation_id); }

void Animator::SetInitialAnimation(const std::string& initial_animation_id)
{
    m_initial_animation_id = initial_animation_id;
}

const Animation& Animator::GetInitialAnimation() const
{
    return m_animations.at(m_initial_animation_id);
}

bool Animator::SwitchToAnimation(const std::string& animation_id)
{
    if (m_animations.count(animation_id) == 0) return false;
    // 变换处理?
    m_current_animation_id = animation_id;
    auto& animation = m_animations.at(animation_id);
    if (!animation.IsLoop() && animation.IsFrameLastIndex()) {
        animation.Restart();
    }
    return true;
}

}  // namespace gameaf