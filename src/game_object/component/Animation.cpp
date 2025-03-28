#include "Animation.h"

#include <common/Log.hpp>
#include <game_object/component/Atlas.hpp>

namespace gameaf {

void Animation::TimerInit()
{
    timer.SetOnTimeout([&]() {
        ++m_frame_index;
        if (m_frame_index >= m_frames.size()) {
            if (m_is_loop) {
                m_frame_index = m_startLoopFrameIndex;
            } else {
                m_frame_index = m_frames.size() - 1;
                if (m_on_finished && !m_isEndOfPlay) {
                    m_on_finished();
                }
                m_isEndOfPlay = true;
            }
        }
    });
}

Animation::Animation() { TimerInit(); }

Animation::Animation(const Animation& animation)
{
    m_frames = animation.m_frames;
    m_frame_index = animation.m_frame_index;
    m_is_loop = animation.m_is_loop;  // 是否是循环动画
    m_on_finished = animation.m_on_finished;
    timer = animation.timer;
    TimerInit();
}

Animation::~Animation() {}

void Animation::OnUpdate(float delta) { timer.OnUpdate(delta); }

void Animation::OnRender(const Camera& camera)
{
    const auto& frame = m_frames.at(m_frame_index);
    frame.img.OnRender(camera, frame.spriteIndex);
}

void Animation::SetImgGameObject(GameObject* obj)
{
    for (auto& frame : m_frames) {
        frame.img.SetGameObject(obj);
    }
}

void Animation::Restart()
{
    timer.Restart();
    m_frame_index = 0;
    m_isEndOfPlay = false;
}

void Animation::SetLoop(bool isLoop) { m_is_loop = isLoop; }

bool Animation::IsLoop() const { return m_is_loop; }

bool Animation::IsEndOfPlay() const { return m_isEndOfPlay; }

void Animation::SetInterval(float interval) { timer.SetWaitTime(interval); }

void Animation::SetOnFinished(std::function<void()> onFinished) { m_on_finished = onFinished; }

bool Animation::AddFrame(const Image& img)
{
    for (size_t i = 0; i < img.GetSpriteNum(); ++i) {
        m_frames.emplace_back(Frame{i, img});
    }
    return true;
}

bool Animation::AddFrame(const Image& img, const std::vector<size_t>& indexs)
{
    for (auto index : indexs) {
        if (index >= img.GetSpriteNum()) {
            gameaf::log("[error][AddFrame] 指定的index不在精灵图总数内...");
            return false;
        }
        m_frames.emplace_back(Frame{index, img});
    }
    return true;
}

bool Animation::AddFrame(Atlas* atlas)
{
    for (int i = 0; i < atlas->Size(); ++i) {
        if (!AddFrame(Image{nullptr, atlas->GetTImg(i)})) return false;
    }
    return true;
}

const Animation::Frame& Animation::GetCurrentFrame() const { return m_frames.at(m_frame_index); }

const Vector2& Animation::CurrentFrameSize() const
{
    return m_frames.at(m_frame_index).img.GetSize();
}

void Animation::Flip()
{
    for (auto& frame : m_frames) {
        frame.img.Flip();
    }
}

void Animation::SetSizeScale(const Vector2& scale)
{
    for (auto& frame : m_frames) {
        frame.img.SetSizeScale(scale);
    }
}

void Animation::SetAnchorMode(ImageAnchorMode mod, const Vector2& anchor_position)
{
    for (auto& frame : m_frames) {
        frame.img.SetAnchorMode(mod, anchor_position);
    }
}

void Animation::SetOffset(const Vector2& offset)
{
    for (auto& frame : m_frames) {
        frame.img.SetOffset(offset);
    }
}

}  // namespace gameaf