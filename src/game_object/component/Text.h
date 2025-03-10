#pragma once

#include <common/Common.h>
#include <game_object/component/Component.h>

#include <unordered_map>

/**
 * 文本组件
 */

namespace gameaf {

class Camera;

// 文本对齐模式
enum class TextAlignMode {
    // Top
    LeftTop,
    CenterTop,
    RightTop,
    // Middle
    LeftMiddle,
    CenterMiddle,
    RightMiddle,
    // Bottom
    LeftBottom,
    CenterBottom,
    RightBottom
};

class Text : public Component
{
public:
    Text(GameObject* obj, const std::wstring& fontName, const Vector2& offset = Vector2{},
         int fontSize = 25, TextAlignMode alignMode = TextAlignMode::LeftTop);
    ~Text();

public:
    void OnRender(const Camera& camera);

public:
    /// @brief 设置文本的字体名字
    void SetFontName(const std::wstring& fontName);

    /// @brief 设置文本的字体大小
    /// @param fontSize 文字像素大小
    void SetFontSize(int fontSize);

    /// @brief 设置文本的对齐模式
    void SetAlignMode(TextAlignMode alignMode);

    /// @brief 获取文本
    const std::wstring& GetText() const { return m_text; }

    /// @brief 设置文本
    void SetText(const std::wstring& wstr);

    /// @brief 设置文本框大小
    /// @param textBoxSize
    void SetTextBoxSize(const Vector2& textBoxSize);

    /// @brief 设置文本颜色
    void SetTextColor(const ColorRGB& textColor) { m_textColor = textColor; }

    /// @brief 文本设置阴影
    void EnableShadow(bool isShaded) { m_isShaded = isShaded; }

    /// @brief 设置文本阴影颜色
    void SetTextShadedColor(const ColorRGB& textShadedColor)
    {
        m_textShadedColor = textShadedColor;
    }

    /// @brief 设置文本阴影偏移
    void SetTextShadedOffset(const Vector2& offset) { m_textShadedOffset = offset; }

private:
    void RenderLeftTop(const Camera& camera, Vector2 pos,
                       std::unordered_map<wchar_t, int>& fontWidthCache);
    void RenderCenterTop(const Camera& camera, Vector2 pos,
                         std::unordered_map<wchar_t, int>& fontWidthCache);

    void PrecomputeTextLayout();  // 预计算文本布局

    // 每行的文本缓存
    struct TextLine
    {
        std::wstring line;  // 行文本缓存
        int lineWidth;      // 每行的宽度
    };

    // 对齐渲染xy坐标的参数
    struct TextAlignArgument
    {
        // 类比 ax + b 的影响
        float a_X, b_X;
        float b_Y;
    };

private:
    std::wstring m_fontName;                 // 字体名字
    int m_fontSize;                          // 字体像素大小
    TextAlignMode m_alignMode;               // 对齐方式
    TextAlignArgument m_alignArgument;       // 对齐参数, 影响渲染时的位置
    std::wstring m_text;                     // 文本内容
    std::vector<TextLine> m_textLinesCache;  // 文本内容行缓存
    Vector2 m_textBoxSize;                   // 文本框大小
    ColorRGB m_textColor;                    // 文本颜色

    bool m_isShaded = false;                            // 是否启用字体阴影
    ColorRGB m_textShadedColor = ColorRGB{45, 45, 45};  // 文本阴影颜色, 默认灰色
    Vector2 m_textShadedOffset = {3.0f, 3.0f};          // 文本阴影偏移
};
}  // namespace gameaf