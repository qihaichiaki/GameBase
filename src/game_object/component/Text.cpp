#include "Text.h"

#include <game_object/GameObject.h>
#include <resource/ResourceCache.h>

#include <common/MediaUtils.hpp>

namespace gameaf {

Text::Text(GameObject* obj, const std::wstring& fontName, const Vector2& offset, int fontSize,
           TextAlignMode alignMode)
    : Component(obj, offset), m_fontName(fontName), m_fontSize(fontSize), m_alignMode(alignMode)
{
    SetFontName(fontName);
}

Text::~Text() {}

void Text::PrecomputeTextLayout()
{
    m_textLinesCache.clear();

    auto& fontWidthCache = ResourceCacheTool::FontWidthCache(m_fontName, m_fontSize);
    gameaf::SetFontSize(m_fontName, m_fontSize);  // 设置字符大小，保证后续计算宽度正确
    // 预计算字符宽度
    for (wchar_t ch : m_text) {
        auto& width = fontWidthCache[ch];
        if (width == 0) width = GetTextWidth(ch);
    }

    // 计算每行文本宽度
    int lineWidth = 0;
    int maxLineWidth = 0;
    std::wstring lineBuffer;

    for (size_t i = 0; i < m_text.size(); ++i) {
        wchar_t ch = m_text[i];

        if (ch == '\n' ||
            (m_textBoxSize.X > 0 && lineWidth + fontWidthCache[ch] > m_textBoxSize.X)) {
            m_textLinesCache.push_back({lineBuffer, lineWidth});
            lineBuffer.clear();
            maxLineWidth = lineWidth > maxLineWidth ? lineWidth : maxLineWidth;
            lineWidth = 0;
            if (ch == '\n') continue;
        }

        // 对高度不做控制, 渲染时做控制
        // if (m_textBoxSize.Y > 0 && lineHeight >= m_textBoxSize.Y) break;
        lineBuffer += ch;
        lineWidth += fontWidthCache[ch];
    }

    if (!lineBuffer.empty()) {
        m_textLinesCache.push_back({std::move(lineBuffer), lineWidth});
        maxLineWidth = lineWidth > maxLineWidth ? lineWidth : maxLineWidth;
    }

    SetAlignMode(m_alignMode);

    // 自适配 m_textBoxSize
    m_renderBoxSize.X = m_textBoxSize.X == 0 ? maxLineWidth : m_textBoxSize.X;
    m_renderBoxSize.Y =
        m_textBoxSize.Y == 0 ? m_textLinesCache.size() * m_fontSize : m_textBoxSize.Y;
}

void Text::SetFontName(const std::wstring& fontName)
{
    if (m_fontName != fontName) {
        m_fontName = fontName;
        PrecomputeTextLayout();
    }
}

void Text::SetFontSize(int fontSize)
{
    if (m_fontSize != fontSize) {
        m_fontSize = fontSize;
        PrecomputeTextLayout();
    }
}

void Text::SetText(const std::wstring& wstr)
{
    if (m_text != wstr) {
        m_text = wstr;
        PrecomputeTextLayout();
    }
}

void Text::SetTextBoxSize(const Vector2& textBoxSize)
{
    if (m_textBoxSize.X != textBoxSize.X) {
        PrecomputeTextLayout();
    } else
        SetAlignMode(m_alignMode);
    m_textBoxSize = textBoxSize;
}

void Text::SetAlignMode(TextAlignMode alignMode)
{
    m_alignMode = alignMode;
    // 相对于中心坐标，计算第N(> 0)行的左上角坐标的影响(用于渲染数据使用)
    // x方向是对于linewidth和固定值的影响，y方向是对于固定值的影响 固定值在下面的[]指示
    // y 方向上:
    // Top   : pos.Y [- m_textBoxSize.Y / 2] + (N - 1) * m_fontSize
    // Middle: pos.Y [+ m_textBoxSize.Y / 2 - m_textLinesCache.size() * m_fontSize * 1.0f / 2] + (N - 1) * m_fontSize
    // Bottom: pos.Y [+ m_textBoxSize.Y / 2 - m_textLinesCache.size() * m_fontSize] + (N - 1) * m_fontSize
    // x 方向上:
    // Left  : pos.X [- m_textBoxSize.X / 2]
    // center: pox.X - linewidth / 2
    // right : pos.X [+ m_textBoxSize.X / 2] - linewidth

    // 影响因素：m_fontSize + m_textLinesCache + m_textBoxSize

    switch (alignMode) {
        // left
        case TextAlignMode::LeftTop:
            m_alignArgument.a_X = 0.0f;
            m_alignArgument.b_X = -m_textBoxSize.X / 2;
            m_alignArgument.b_Y = -m_textBoxSize.Y / 2;
            break;
        case TextAlignMode::CenterTop:
            m_alignArgument.a_X = -0.5f;
            m_alignArgument.b_X = 0.0f;
            m_alignArgument.b_Y = -m_textBoxSize.Y / 2;
            break;
        case TextAlignMode::RightTop:
            m_alignArgument.a_X = -1.0f;
            m_alignArgument.b_X = m_textBoxSize.X / 2;
            m_alignArgument.b_Y = -m_textBoxSize.Y / 2;
            break;
        case TextAlignMode::LeftMiddle:
            m_alignArgument.a_X = 0.0f;
            m_alignArgument.b_X = -m_textBoxSize.X / 2;
            m_alignArgument.b_Y =
                -static_cast<int>(m_textLinesCache.size()) * m_fontSize * 1.0f / 2;
            break;
        case TextAlignMode::CenterMiddle:
            m_alignArgument.a_X = -0.5f;
            m_alignArgument.b_X = 0.0f;
            m_alignArgument.b_Y =
                -static_cast<int>(m_textLinesCache.size()) * m_fontSize * 1.0f / 2;
            break;
        case TextAlignMode::RightMiddle:
            m_alignArgument.a_X = -1.0f;
            m_alignArgument.b_X = m_textBoxSize.X / 2;
            m_alignArgument.b_Y =
                -static_cast<int>(m_textLinesCache.size()) * m_fontSize * 1.0f / 2;
            break;
        case TextAlignMode::LeftBottom:
            m_alignArgument.a_X = 0.0f;
            m_alignArgument.b_X = -m_textBoxSize.X / 2;
            m_alignArgument.b_Y =
                m_textBoxSize.Y / 2 - static_cast<int>(m_textLinesCache.size()) * m_fontSize;
            break;
        case TextAlignMode::CenterBottom:
            m_alignArgument.a_X = -0.5f;
            m_alignArgument.b_X = 0.0f;
            m_alignArgument.b_Y =
                m_textBoxSize.Y / 2 - static_cast<int>(m_textLinesCache.size()) * m_fontSize;
            break;
        case TextAlignMode::RightBottom:
            m_alignArgument.a_X = -1.0f;
            m_alignArgument.b_X = m_textBoxSize.X / 2;
            m_alignArgument.b_Y =
                m_textBoxSize.Y / 2 - static_cast<int>(m_textLinesCache.size()) * m_fontSize;
            break;
    }
}

void Text::OnRender(const Camera& camera)
{
    gameaf::SetFontSize(m_fontName, m_fontSize);

    int lineHeight = 0;
    Vector2 pos = Position();
    Vector2 linePos;
    for (const auto& [line, lineWidth] : m_textLinesCache) {
        linePos.X = pos.X + m_alignArgument.a_X * lineWidth + m_alignArgument.b_X;
        linePos.Y = pos.Y + m_alignArgument.b_Y + lineHeight;
        if (m_isShaded)
            PutTextShaded(camera, line, linePos, lineWidth, m_fontSize, m_textColor,
                          m_textShadedColor, m_textShadedOffset);
        else
            PutText(camera, line, linePos, lineWidth, m_fontSize, m_textColor);
        lineHeight += m_fontSize;
        if (m_textBoxSize.Y > 0 && lineHeight >= m_textBoxSize.Y) break;
    }
}

bool Text::ContainsScreenPoint(const Camera& camera, const Vector2& pos) const
{
    Vector2 textPos = Position();
    textPos.X += m_alignArgument.a_X * m_renderBoxSize.X + m_alignArgument.b_X;
    textPos.Y += m_alignArgument.b_Y;
    float x = textPos.X - camera.GetPosition().X;
    float y = textPos.Y - camera.GetPosition().Y;
    return (pos.X >= x && pos.X <= x + m_renderBoxSize.X) &&
           (pos.Y >= y && pos.Y <= y + m_renderBoxSize.Y);
}

void Text::OnDebugRender(const Camera& camera)
{
    Vector2 position = Position();
    int left = static_cast<int>(position.X - m_renderBoxSize.X / 2 - camera.GetPosition().X);
    int top = static_cast<int>(position.Y - m_renderBoxSize.Y / 2 - camera.GetPosition().Y);
    int right = static_cast<int>(position.X + m_renderBoxSize.X / 2 - camera.GetPosition().X);
    int bottom = static_cast<int>(position.Y + m_renderBoxSize.Y / 2 - camera.GetPosition().Y);
#ifdef GAMEAF_USE_EASYX
    setlinecolor(RGB(115, 115, 175));
    rectangle(left, top, right, bottom);
#else
#endif
}

}  // namespace gameaf