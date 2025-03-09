#include "Text.h"

#include <game_object/GameObject.h>
#include <resource/ResourceCache.h>

#include <common/MediaUtils.hpp>

namespace gameaf {

Text::Text(GameObject* obj, const std::wstring& fontName, const Vector2& offset, int fontSize,
           TextAlignMode alignMode)
    : Component(obj, offset), m_fontName(fontName), m_fontSize(fontSize), m_alignMode(alignMode)
{
}

Text::~Text() {}

inline static void RenderLeftTop(const Camera& camera, Vector2 pos, const Vector2& textBoxSize,
                                 int fontSize, std::unordered_map<wchar_t, int>& fontWidthCache,
                                 const std::wstring& text)
{
    int lineWidth = 0;
    int lineHeight = 0;
    int subStart = 0;
    std::wostringstream lineBuffer;
    // 计算左上角位置
    pos.X -= textBoxSize.X / 2;
    pos.Y -= textBoxSize.Y / 2;

    // 预计算字符宽度，避免 `GetTextWidth()` 频繁调用
    for (char ch : text) {
        if (fontWidthCache.find(ch) == fontWidthCache.end()) {
            fontWidthCache[ch] = GetTextWidth(ch);
        }
    }

    for (int i = 0; i < text.size(); ++i) {
        wchar_t ch = text[i];
        if (ch == '\n' || (textBoxSize.X > 0 && lineWidth + fontWidthCache[ch] > textBoxSize.X)) {
            PutTextShaded(camera, lineBuffer.str(), Vector2{pos.X, pos.Y + lineHeight});
            lineBuffer.str(L"");
            lineWidth = 0;
            lineHeight += fontSize;

            if (ch == '\n') continue;
        }

        if (textBoxSize.Y != 0 && lineHeight >= textBoxSize.Y) break;

        lineBuffer << ch;
        lineWidth += fontWidthCache[ch];
    }

    if (!lineBuffer.str().empty()) {
        PutTextShaded(camera, lineBuffer.str(), Vector2{pos.X, pos.Y + lineHeight});
    }
}

void Text::OnRender(const Camera& camera)
{
    gameaf::SetFontSize(m_fontName, m_fontSize);

    auto& fontWidthCache = ResourceCacheTool::FontWidthCache(m_fontName, m_fontSize);
    switch (m_alignMode) {
        case TextAlignMode::LeftTop:  // 左上
            RenderLeftTop(camera, m_gameObject->GetPosition() + m_offset, m_textBoxSize, m_fontSize,
                          fontWidthCache, m_text);
            break;
        case TextAlignMode::CenterTop:  // 中上
            break;
        case TextAlignMode::RightTop:  // 右上
            break;
        case TextAlignMode::LeftMiddle:  // 左中
            break;
        case TextAlignMode::CenterMiddle:  // 中
            break;
        case TextAlignMode::RightMiddle:  // 右中
            break;
        case TextAlignMode::LeftBottom:  // 左下
            break;
        case TextAlignMode::CenterBottom:  // 中下
            break;
        case TextAlignMode::RightBottom:  // 右下
            break;
        default:
            break;
    }
}

}  // namespace gameaf