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
public:
    friend class Animator;
    // 描述动画里的一帧
    struct Frame
    {
        size_t spriteIndex;  // 精灵图中精灵图id
        Image img;           // 图像组件
    };

public:
    Animation();
    Animation(const Animation&);
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
    void SetOnFinished(std::function<void(Animation*)> onFinished);

    /// @brief 将动画的每帧的图像添加
    /// @return 添加帧是否成功
    bool AddFrame(const Image& img);

    /// @brief 将精灵图中特定的某些图像添加到帧内
    /// @param img 精灵图
    /// @param indexs 精灵图中每帧图在拆分中的编号(从0开始)
    bool AddFrame(const Image& img, const std::vector<size_t>& indexs);

    /// @brief 获取当前帧
    const Frame& GetCurrentFrame() const;

    /// @brief 当前帧的大小
    const Vector2& CurrentFrameSize() const;

    /// @brief x方向上垂直旋转自身
    void Flip();

    /// @brief 设置动画每帧的图像大小比例
    void SetSizeScale(const Vector2& scale);

    /// @brief 设置锚点模式
    /// @param mod 锚点模式
    /// @param anchor_position 如果自定义,
    /// 请自由的设置锚点在x方向所占的比例和在y方向所占的比例(单位矩形).值的取值在[0,1]
    void SetAnchorMode(ImageAnchorMode mod, const Vector2& anchor_position = {0.0f, 0.0f});

    /// @brief 设置循环从多少帧开始
    void SetLoopBeginFrameIndex(int startLoopFrameIndex)
    {
        m_startLoopFrameIndex = startLoopFrameIndex;
    }

private:
    void TimerInit();

private:
    std::vector<Frame> m_frames;
    int m_frame_index = 0;
    int m_startLoopFrameIndex = 0;  // 从多少帧开始循环, 默认一开始就循环
    Timer timer;
    bool m_is_loop = false;                         // 是否是循环动画
    std::function<void(Animation*)> m_on_finished;  // 驱动力
};
}  // namespace gameaf
