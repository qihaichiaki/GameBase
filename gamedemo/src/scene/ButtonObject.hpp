#pragma once
#include <game_object/GameObject.h>
#include <game_object/component/Animator.h>
#include <game_object/component/Text.h>
#include <resource/ResourceManager.h>

#include <common/Log.hpp>
#include <functional>

#include "../Common.hpp"

using gameaf::Animator;
using gameaf::ColorRGB;
using gameaf::GameObject;
using gameaf::ResourceManager;
using gameaf::Text;
using gameaf::TextAlignMode;
using gameaf::Vector2;

class UiChoose : public GameObject
{
public:
    void OnAwake() override
    {
        animator = CreateComponent<Animator>();
        animator->AddAnimationForAtlas("idle", "ui_choose", false, 0.08f);
    }

    GameObject::GameObjectPtr Clone() override
    {
        auto clonePtr = std::make_shared<UiChoose>(*this);
        clonePtr->animator = clonePtr->GetComponent<Animator>();
        if (auto parent = GetParent()) {
            parent->AddChildObject(clonePtr);
        }
        return clonePtr;
    }

    void OnEnter() override { animator->Restart(); }

private:
    Animator* animator;
};

class Button : public GameObject
{
public:
    Button(const std::wstring& text) : GameObject("button"), text(text) {}

    void OnAwake() override
    {
        SetZOrder(RenderZOrder::UI_1);
        textComponent = CreateComponent<Text>(std::wstring{L"Syne Mono"});

        textComponent->SetText(text);
        textComponent->SetAlignMode(TextAlignMode::CenterTop);
        textComponent->SetFontSize(60);

        ui_chooseLeft = std::make_shared<UiChoose>();

        ui_chooseLeft->SetPosition({GetPosition().X - 140.0f, GetPosition().Y + 25.0f});
        AddChildObject(ui_chooseLeft);

        ui_chooseRight = ui_chooseLeft->Clone();
        ui_chooseRight->Flip();
        ui_chooseRight->Translate({280.0f, 0.0f});
        Select(false);
    }

    void OnMouseEnter() override
    {
        Select(true);
        textComponent->SetTextColor(ColorRGB{255, 0, 0});
    }

    void OnMouseExit() override
    {
        Select(false);
        textComponent->SetTextColor(ColorRGB{});
    }

    void OnMouseClicked() override
    {
        if (onMouseClicked) onMouseClicked();
    }

public:
    void Select(bool isSelect)
    {
        ui_chooseLeft->SetActive(isSelect);
        ui_chooseRight->SetActive(isSelect);
    }

    void RegisterMouseClicked(std::function<void()> mouseClicked) { onMouseClicked = mouseClicked; }

private:
    std::wstring text;
    Text* textComponent;
    std::shared_ptr<GameObject> ui_chooseLeft;
    std::shared_ptr<GameObject> ui_chooseRight;
    std::function<void()> onMouseClicked;
};