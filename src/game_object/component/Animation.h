#pragma once

#include <game_object/component/Image.h>

#include <common/Vector2.hpp>
#include <functional>
#include <game_object/component/Timer.hpp>

namespace gameaf {

class Camera;
class GameObject;
class Animation
{
    friend class Animator;

public:
    Animation();
    ~Animation();

public:
    void OnUpdate(float delta);
    void OnRender(const Camera& camera);
    void SetImgGameObject(GameObject* obj);

public:
    /// @brief 重放
    void Restart();

    /// @brief 设置是否循环
    /// @param isLoop 是否循环
    void SetLoop(bool isLoop);

    /// @brief 查询是否循环
    bool IsLoop() const;

    /// @brief 是否为动画帧的最后一帧?
    bool IsFrameLastIndex() const;

    /// @brief 设置动画在多少s后播放一帧
    /// @param interval s
    void SetInterval(float interval);

    /// @brief 设置动画最后一帧结束后执行的回调
    /// @param onFinished 执行回调逻辑
    void SetOnFinished(std::function<void()> onFinished);

    /// @brief 将动画的每帧的图像添加
    /// @return 添加帧是否成功
    bool AddFrame(const Image& img);

    /// @brief 当前帧的大小
    const Vector2& CurrentFrameSize() const;

    /// @brief x方向上垂直旋转自身
    void Flip();

private:
    // 描述动画里的一帧
    struct Frame
    {
        size_t spriteIndex;  // 精灵图中精灵图id
        Image img;           // 图像组件
    };

    std::vector<Frame> m_frames;
    int m_frame_index = 0;
    Timer timer;
    bool m_is_loop = false;               // 是否是循环动画
    std::function<void()> m_on_finished;  // 驱动力
};
}  // namespace gameaf
