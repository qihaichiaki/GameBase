#include "animator.h"

#include <resource_manager.h>

namespace gameaf {

Animator::Animator() = default;
Animator::~Animator() = default;

void Animator::onUpdate(float delta) { m_animations.at(m_current_animation_id).onUpdate(delta); }

void Animator::onRender(const Camera& camera, const Rect& rect)
{
    m_animations.at(m_current_animation_id).onRender(camera, rect);
}

bool Animator::newAnimationForImage(const std::string& animation_id, const std::string& img_id,
                                    float interval, bool is_loop)
{
    if (m_animations.count(animation_id) != 0) return false;

    Image* img = ResourceManager::getInstance().getImage(img_id);
    if (img == nullptr) return false;
    m_animations[animation_id].addFrame(img);
    m_animations[animation_id].setInterval(interval);
    m_animations[animation_id].setLoop(is_loop);
    return true;
}

bool Animator::newAnimationForAtlas(const std::string& animation_id, const std::string& atlas_id,
                                    float interval, bool is_loop)
{
    if (m_animations.count(animation_id) != 0) return false;

    Atlas* atlas = ResourceManager::getInstance().getAtlas(atlas_id);
    if (atlas == nullptr) return false;
    m_animations[animation_id].addFrame(atlas);
    m_animations[animation_id].setInterval(interval);
    m_animations[animation_id].setLoop(is_loop);
    return true;
}

bool Animator::delAnimation(const std::string& animation_id)
{
    if (m_animations.count(animation_id) == 0) return false;

    m_animations.erase(animation_id);
    return true;
}

}  // namespace gameaf