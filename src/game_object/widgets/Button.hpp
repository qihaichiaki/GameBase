#pragma once

#include <game_object/GameObject.h>
#include <game_object/component/Text.h>

#include <functional>

/**
 * 按键, 基于GameObject的UI组件
 */

namespace gameaf {
class Button : public GameObject
{
public:
    Button(const std::wstring& fontName, const std::wstring& text = L"Button")
    {
        m_text = CreateComponent<Text>(fontName);
        m_text->SetText(text);
        m_text->SetAlignMode(TextAlignMode::CenterMiddle);
        m_text->SetFontSize(60);
    }

    /// @brief 获取text组件
    Text* GetTextComponent() const { return m_text; }

    /// @brief 注册鼠标点击回调
    void RegisterMouseClicked(std::function<void()> mouseClicked) { onMouseClicked = mouseClicked; }

public:
    GameObjectPtr Clone() override
    {
        auto clonePtr = std::make_shared<Button>(*this);
        if (auto parent = GetParent()) {
            parent->AddChildObject(clonePtr);
        }
        return clonePtr;
    }

    void OnMouseUp() override
    {
        if (onMouseClicked) onMouseClicked();
    }

private:
    Text* m_text = nullptr;
    std::function<void()> onMouseClicked;
};
}  // namespace gameaf