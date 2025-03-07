#pragma once

#include <game_object/component/Component.h>

/**
 * 文本组件
 */

namespace gameaf {

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
    Text(GameObject* obj, const Vector2& offset);
    ~Text();

public:
private:
    TextAlignMode m_alignMode = TextAlignMode::LeftTop;
};
}  // namespace gameaf