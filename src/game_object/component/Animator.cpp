#include "Animator.h"

#include <common/Common.h>
#include <game_object/component/Image.h>
#include <resource/ResourceManager.h>

#include <common/Log.hpp>
#include <game_object/component/Atlas.hpp>

namespace gameaf {

Animator::Animator(GameObject* obj) : Component(obj, Vector2{}) {};
Animator::~Animator() = default;

void Animator::OnUpdate(float delta) { m_animations.at(m_current_animation_id).OnUpdate(delta); }

void Animator::OnRender(const Camera& camera)
{
    m_animations.at(m_current_animation_id).OnRender(camera);
}

void Animator::SetGameObject(GameObject* obj)
{
    m_gameObject = obj;
    for (auto& [_, animation] : m_animations) {
        animation.SetImgGameObject(obj);
    }
}

void Animator::Flip()
{
    for (auto& [_, animation] : m_animations) {
        animation.Flip();
    }
}

bool Animator::AddAnimationForImage(const std::string& animation_id, const std::string& imgID,
                                    bool isLoop, float interval)
{
    if (m_animations.count(animation_id) != 0) {
        gameaf::log("[warring][AddAnimation] animation重名了...");
        return false;
    }
    auto timg = ResourceManager::GetInstance().GetTImage(imgID);
    if (timg == nullptr) {
        gameaf::log("[error][AddAnimation] {}-图像元空空如也...", imgID);
        return false;
    }

    if (m_initial_animation_id == "") {
        m_initial_animation_id = animation_id;
        m_current_animation_id = m_initial_animation_id;
    }
    if (!m_animations[animation_id].AddFrame(Image{m_gameObject, timg, m_offset})) return false;
    m_animations[animation_id].SetLoop(isLoop);
    m_animations[animation_id].SetInterval(interval);
    return true;
}

bool Animator::AddAnimationForImage(const std::string& animation_id, const std::string& imgID,
                                    const std::vector<size_t>& indexs, bool isLoop, float interval)
{
    if (m_animations.count(animation_id) != 0) {
        gameaf::log("[warring][AddAnimation] animation重名了...");
        return false;
    }
    auto timg = ResourceManager::GetInstance().GetTImage(imgID);
    if (timg == nullptr) {
        gameaf::log("[error][AddAnimation] {}-图像元空空如也...", imgID);
        return false;
    }

    if (m_initial_animation_id == "") {
        m_initial_animation_id = animation_id;
        m_current_animation_id = m_initial_animation_id;
    }
    if (!m_animations[animation_id].AddFrame(Image{m_gameObject, timg, m_offset}, indexs))
        return false;
    m_animations[animation_id].SetLoop(isLoop);
    m_animations[animation_id].SetInterval(interval);
    return true;
}

bool Animator::AddAnimationForImage(const std::string& animation_id, const std::string& imgID,
                                    size_t begin, size_t len, bool isLoop, float interval)
{
    std::vector<size_t> indexs;
    indexs.reserve(len);
    for (size_t i = begin; i < begin + len; ++i) {
        indexs.push_back(i);
    }
    return AddAnimationForImage(animation_id, imgID, indexs, isLoop, interval);
}

bool Animator::AddAnimationForAtlas(const std::string& animation_id, const std::string& atlasID,
                                    bool isLoop, float interval)
{
    if (m_animations.count(animation_id) != 0) {
        gameaf::log("[warring][AddAnimation] animation重名了...");
        return false;
    }
    auto atlas = ResourceManager::GetInstance().GetAtlas(atlasID);
    if (atlas == nullptr) {
        gameaf::log("[error][AddAnimation] {}-图集空空如也...", atlasID);
        return false;
    }

    if (m_initial_animation_id == "") {
        m_initial_animation_id = animation_id;
        m_current_animation_id = m_initial_animation_id;
    }
    for (int i = 0; i < atlas->Size(); ++i) {
        if (!m_animations[animation_id].AddFrame(Image{m_gameObject, atlas->GetTImg(i), m_offset}))
            return false;
    }
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

bool Animator::ContainsScreenPoint(const Camera& camera, const Vector2& pos) const
{
    return m_animations.at(m_initial_animation_id)
        .GetCurrentFrame()
        .img.ContainsScreenPoint(camera, pos);
}

void Animator::Restart()
{
    for (auto& [_, animation] : m_animations) {
        animation.Restart();
    }
}

void Animator::SetSizeScale(const Vector2& scale)
{
    for (auto& [_, animation] : m_animations) {
        animation.SetSizeScale(scale);
    }
}

}  // namespace gameaf