#pragma once

#include "../../common/media_utils.hpp"
#include "atlas.hpp"
#include "timer.hpp"

namespace gameaf {

class Camera;

class Animation
{
public:
    Animation()
    {
        timer.setOnTimeout([&]() {
            ++m_frame_index;
            if (m_frame_index >= m_frames.size()) {
                m_frame_index = m_is_loop ? 0 : m_frames.size() - 1;
                if (m_on_finished) m_on_finished();
            }
        });
    }

    ~Animation() = default;

public:
    void onUpdate(float delta) { timer.onUpdate(delta); }

    void onRender(const Camera& camera, Rect dst)
    {
        const auto& frame = m_frames.at(m_frame_index);
        putImageEx(camera, *frame.img, dst, frame.src_rect);
    }

public:
    void restart()
    {
        timer.restart();
        m_frame_index = 0;
    }

    /// @brief 设置是否循环
    /// @param isLoop 循环?
    void setLoop(bool isLoop) { m_is_loop = isLoop; }

    /// @brief 设置动画在多少s后播放一帧
    /// @param interval s
    void setInterval(float interval) { timer.setWaitTime(interval); }

    /// @brief 设置动画最后一帧结束后执行的回调
    /// @param onFinished 执行回调逻辑
    void setOnFinished(std::function<void()> onFinished) { m_on_finished = onFinished; }

    /// @brief 将切分好的img精灵图加载到动画的帧内
    void addFrame(Image* img)
    {
        for (int i = 0; i < img->getSpriteNum(); ++i) {
            Frame frame;
            auto src_rect = img->getSpriteRect(i);
            frame.src_rect = {(float)src_rect.x, (float)src_rect.y, src_rect.w, src_rect.h};
            frame.img = img;
            m_frames.emplace_back(frame);
            if (i == 0) {
                // 初始化动画每frame的原始大小
                // WARRING: 注意动画不会区分每帧的适配大小
                m_size = {src_rect.w * 1.0f, src_rect.h * 1.0f};
            }
        }
    }

    void addFrame(Atlas* atlas)
    {
        for (int i = 0; i < atlas->size(); ++i) {
            Frame frame;
            frame.img = atlas->getImg(i);
            frame.src_rect = {0.f, 0.f, frame.img->getWidth(), frame.img->getHeight()};
            m_frames.emplace_back(frame);
            if (i == 0) {
                // 同上
                m_size = {frame.img->getWidth() * 1.0f, frame.img->getHeight() * 1.0f};
            }
        }
    }

    Vector2& size() { return m_size; }
    const Vector2& size() const { return m_size; }

private:
    // 描述动画里的一帧
    struct Frame
    {
        Rect src_rect;
        Image* img;
    };

    std::vector<Frame> m_frames;
    Vector2 m_size;  // 此动画每frame适配的大小
    int m_frame_index = 0;
    Timer timer;
    bool m_is_loop = false;               // 是否是循环动画
    std::function<void()> m_on_finished;  // 驱动力
};
}  // namespace gameaf
