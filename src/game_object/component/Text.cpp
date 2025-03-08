#include "Text.h"

#include <game_object/GameObject.h>

#include <common/MediaUtils.hpp>

namespace gameaf {

Text::Text(GameObject* obj, const std::string& fontName, const Vector2& offset, int fontSize,
           TextAlignMode alignMode)
    : Component(obj, offset), m_fontName(fontName), m_fontSize(fontSize), m_alignMode(alignMode)
{
}

Text::~Text() {}

inline static void RenderLeftTop(const Camera& camera, Vector2 pos, const Vector2& textBoxSize,
                                 int fontSize, const std::string& text)
{
    int lineWidth = 0;
    int lineHeight = 0;
    int subStart = 0;
    // 计算左上角位置
    pos.X = pos.X - textBoxSize.X / 2;
    pos.Y = pos.Y - textBoxSize.Y / 2;

    if (textBoxSize == Vector2{}) {
        // 自适应
        for (int i = 0; i < text.size(); ++i) {
            if (text[i] == '\n') {
                PutTextShaded(camera, text.substr(subStart, i - subStart),
                              Vector2{pos.X, pos.Y + lineHeight});
                subStart = i + 1;
                lineHeight += fontSize;
            }

            if (i + 1 == text.size()) {
                PutTextShaded(camera, text.substr(subStart), Vector2{pos.X, pos.Y + lineHeight});
            }
        }
    } else if (textBoxSize.X != 0) {
        // 存在文本框限制
        for (int i = 0; i < text.size(); ++i) {
            // 当前列
            if (lineHeight >= textBoxSize.Y) {
                break;
            }

            if (text[i] != '\n') {
                lineWidth += GetTextWidth(text[i]);
                // 如果当前行超出范围
                if (lineWidth > textBoxSize.X) {
                    PutTextShaded(camera, text.substr(subStart, i - subStart),
                                  Vector2{pos.X, pos.Y + lineHeight});
                    subStart = i;
                    lineWidth = GetTextWidth(text[i]);
                    lineHeight += fontSize;
                }
            } else {
                PutTextShaded(camera, text.substr(subStart, i - subStart),
                              Vector2{pos.X, pos.Y + lineHeight});
                subStart = i + 1;
                lineWidth = 0;
                lineHeight += fontSize;
            }

            if (i + 1 == text.size()) {
                PutTextShaded(camera, text.substr(subStart), Vector2{pos.X, pos.Y + lineHeight});
            }
        }
    }
}

void Text::OnRender(const Camera& camera)
{
    gameaf::SetFontSize(m_fontName, m_fontSize);

    switch (m_alignMode) {
        case TextAlignMode::LeftTop:  // 左上
            RenderLeftTop(camera, m_gameObject->GetPosition() + m_offset, m_textBoxSize, m_fontSize,
                          m_text);
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