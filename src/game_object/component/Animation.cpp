#include "Animation.h"

#include <common/Log.hpp>

namespace gameaf {

Animation::Animation()
{
    timer.SetOnTimeout([&]() {
        ++m_frame_index;
        if (m_frame_index >= m_frames.size()) {
            m_frame_index = m_is_loop ? 0 : m_frames.size() - 1;
            if (m_on_finished) m_on_finished();
        }
    });
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
}

void Animation::SetLoop(bool isLoop) { m_is_loop = isLoop; }

bool Animation::IsLoop() const { return m_is_loop; }

bool Animation::IsFrameLastIndex() const { return m_frames.size() == m_frame_index + 1; }

void Animation::SetInterval(float interval) { timer.SetWaitTime(interval); }

void Animation::SetOnFinished(std::function<void()> onFinished) { m_on_finished = onFinished; }

bool Animation::AddFrame(const Image& img)
{
    for (size_t i = 0; i < img.GetSpriteNum(); ++i) {
        m_frames.emplace_back(Frame{i, img});
    }

    return true;
}

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

}  // namespace gameaf