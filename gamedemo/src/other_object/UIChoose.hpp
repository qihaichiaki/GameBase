#pragma once
#include <game_object/GameObject.h>
#include <game_object/component/Animator.h>

#include <common/Log.hpp>
#include <game_object/widgets/Button.hpp>

#include "../Common.hpp"

using gameaf::Animator;
using gameaf::Button;
using gameaf::Camera;
using gameaf::ColorRGB;
using gameaf::GameObject;
using gameaf::Text;
using gameaf::Vector2;

class UIChooseAnimator : public GameObject
{
public:
    UIChooseAnimator() : GameObject("UIChooseAnimator") {}

    void OnAwake() override
    {
        animator = CreateComponent<Animator>();
        animator->AddAnimationForAtlas("idle", "ui_choose", false, 0.08f);
    }

    GameObject::GameObjectPtr Clone() override
    {
        auto clonePtr = std::make_shared<UIChooseAnimator>(*this);
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

class UIChoose : public Button
{
public:
    UIChoose(const std::wstring& text) : Button(L"Syne Mono", text) {}

    void OnAwake() override
    {
        SetZOrder(RenderZOrder::UI_1);

        ui_chooseLeft = std::make_shared<UIChooseAnimator>();

        ui_chooseLeft->SetPosition({-GetTextComponent()->GetRenderBoxSize().X / 2 - 40.0f, -5.0f});
        AddChildObject(ui_chooseLeft);

        ui_chooseRight = ui_chooseLeft->Clone();
        ui_chooseRight->Flip();
        ui_chooseRight->Translate({GetTextComponent()->GetRenderBoxSize().X + 80.0f, 0.0f});
        Select(false);
    }

    void OnMouseEnter() override
    {
        Select(true);
        GetTextComponent()->SetTextColor(ColorRGB{255, 0, 0});
    }

    void OnMouseExit() override
    {
        Select(false);
        GetTextComponent()->SetTextColor(ColorRGB{});
    }

    // void OnDraw(const Camera& camera) { GetTextComponent()->OnDebugRender(camera); }

public:
    void Select(bool isSelect)
    {
        ui_chooseLeft->SetActive(isSelect);
        ui_chooseRight->SetActive(isSelect);
    }

private:
    std::wstring text;
    std::shared_ptr<GameObject> ui_chooseLeft;
    std::shared_ptr<GameObject> ui_chooseRight;
};