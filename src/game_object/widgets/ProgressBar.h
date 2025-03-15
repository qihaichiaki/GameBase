#pragma once

#include <common/Common.h>
#include <game_object/GameObject.h>

/**
 * 进度条 - 基于gameobject的UI控件
 */

namespace gameaf {

class ProgressBar : public GameObject
{
public:
    ProgressBar() = default;
    ProgressBar(const Vector2& backgroundSize, const Vector2& progressBarSize)
        : m_borderSize(backgroundSize), m_progressBarSize(progressBarSize)
    {
    }

    /// @brief 设置外边框大小
    void SetBorderSize(const Vector2& borderSize) { m_borderSize = borderSize; }

    /// @brief 设置进度条大小
    void SetProgressBarSize(const Vector2& progressBarSize) { m_progressBarSize = progressBarSize; }

    /// @brief 设置是否存在滞留进度
    /// @param ghostSpeedScale 滞留条相对于进度条的速度
    void SetGhostBar(bool isGhostBar, float ghostSpeedScale = 0.5f)
    {
        m_isGhostBar = isGhostBar;
        m_ghostSpeedScale = std::clamp(ghostSpeedScale, 0.0f, 1.0f);
    }

    /// @brief 直接设置进度(0,0 ~ 1.0)
    void SetProgressValue(float progressValue)
    {
        m_progressValue = std::clamp(progressValue, 0.0f, 1.0f);
        m_targetProgressValue = m_progressValue;
    }

    /// @brief 设置模板进度值(从当前进度到目标进度会有一个简单动画过渡)
    void SetTargetProgressValue(float progressValue)
    {
        m_targetProgressValue = std::clamp(progressValue, 0.0f, 1.0f);
    }

    /// @brief 设置进度条颜色
    void SetProgressBarColor(const ColorRGB& rogressBarColor)
    {
        m_progressBarColor = rogressBarColor;
    }

    /// @brief 设置滞留条颜色
    void SetGhostBarColor(const ColorRGB& ghostBarColor) { m_ghostBarColor = ghostBarColor; }

    /// @brief 设置背景颜色
    void SetBackgroundColor(const ColorRGB& backgroundColor)
    {
        m_backgroundColor = backgroundColor;
    }

    /// @brief 设置外边框颜色
    void SetBorderColorColor(const ColorRGB& borderColor) { m_borderColor = borderColor; }

public:
    GameObjectPtr Clone() override
    {
        auto clonePtr = std::make_shared<ProgressBar>(*this);
        if (auto parent = GetParent()) {
            parent->AddChildObject(clonePtr);
        }
        return clonePtr;
    }

    void OnUpdate() override;

    void OnDraw(const Camera& camera) override;

private:
    Vector2 m_borderSize = {400.0f, 20.0f};         // 进度条外边框的大小
    Vector2 m_progressBarSize = {394.0f, 18.0f};    // 进度条实际大小
    int m_ellipsewidth = 10, m_ellipseheight = 10;  // 进度条显示圆角

    ColorRGB m_borderColor{255, 255, 255};   // 外边框颜色
    ColorRGB m_backgroundColor{45, 45, 45};  // 背景颜色
    ColorRGB m_progressBarColor{"#c6ebff"};  // 进度条颜色
    ColorRGB m_ghostBarColor{"#82d4ff"};     // 滞留条颜色

    // 是否启用滞留进度(减少时, 主进度条不在有动画效果, 存在滞留进度条有动画效果)
    bool m_isGhostBar = false;
    float m_progressValue = 0.0f;        // 当前进度值 0 ~ 1.0
    float m_ghostProgressValue = 0.0f;   // 滞留进度值
    float m_targetProgressValue = 0.0f;  // 目标进度值
    float m_progressSpeed = 0.28f;       // 进度条速度
    float m_ghostSpeedScale = 0.5f;      // 滞留速度占比(0.0~1.0) 对应为speed的多少倍
};
}  // namespace gameaf