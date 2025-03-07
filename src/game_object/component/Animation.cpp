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
    frame.img->OnRender(camera, frame.spriteIndex);
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

bool Animation::AddFrame(Image* img)
{
    if (img == nullptr) {
        gameaf::log("[error][Animation::AddFrame] 添加帧失败...");
        return false;
    }

    // img -> gameObject ?
    img->SetGameObject(m_gameObject);
    for (int i = 0; i < img->GetSpriteNum(); ++i) {
        Frame frame;
        auto src_rect = img->GetSpriteRect(i);
        frame.spriteIndex = i;
        frame.img = img;
        m_frames.emplace_back(frame);
    }

    return true;
}

bool Animation::AddFrame(Atlas* atlas)
{
    if (atlas == nullptr) {
        gameaf::log("[error][Animation::AddFrame] 添加帧失败...");
        return false;
    }

    for (int i = 0; i < atlas->Size(); ++i) {
        Frame frame;
        frame.spriteIndex = 0;
        frame.img = atlas->GetImg(i);
        frame.img->SetGameObject(m_gameObject);
        m_frames.emplace_back(frame);
    }

    return true;
}

const Vector2& Animation::CurrentFrameSize() const
{
    return m_frames.at(m_frame_index).img->GetSize();
}

}  // namespace gameaf