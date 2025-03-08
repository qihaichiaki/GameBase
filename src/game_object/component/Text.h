#pragma once

#include <game_object/component/Component.h>

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
    Text(GameObject* obj, const std::string& fontName, const Vector2& offset = Vector2{},
         int fontSize = 25, TextAlignMode alignMode = TextAlignMode::LeftTop);
    ~Text();

public:
    void OnRender(const Camera& camera);

public:
    /// @brief 设置文本的字体名字
    void SetFontName(const std::string& fontName) { m_fontName = fontName; }

    /// @brief 设置文本的字体大小
    /// @param fontSize 文字像素大小
    void SetFontSize(int fontSize) { m_fontSize = fontSize; }

    /// @brief 设置文本的对齐模式
    void SetAlignMode(TextAlignMode alignMode) { m_alignMode = alignMode; }

    /// @brief 获取文本
    const std::string& GetStr() const { return m_text; }

    /// @brief 设置文本
    void SetStr(const std::string& str) { m_text = str; }

    /// @brief 设置文本框大小
    /// @param textBoxSize
    void SetTextBoxSize(const Vector2& textBoxSize) { m_textBoxSize = textBoxSize; }

private:
    std::string m_fontName;     // 字体名字
    int m_fontSize;             // 字体像素大小
    TextAlignMode m_alignMode;  // 对齐方式
    std::string m_text;         // 文本内容
    Vector2 m_textBoxSize;      // 文本框大小
};
}  // namespace gameaf