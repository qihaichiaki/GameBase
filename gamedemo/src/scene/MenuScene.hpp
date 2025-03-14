#pragma once

#include <game_object/GameObject.h>
#include <game_object/component/AudioManager.h>
#include <game_object/component/Text.h>
#include <resource/ResourceManager.h>
#include <scene/Scene.h>

#include <common/Log.hpp>

#include "../Common.hpp"
#include "EffectObject.hpp"

using namespace gameaf;

class MenuScene : public Scene
{
public:
    void OnAwake() override
    {
        // 初始化时调用
        // 创建标题对象
        auto title = std::make_shared<GameObject>(RenderZOrder::UI_1, "title");
        auto titleText = title->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        auto subheading = title->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        // 设置到场景主相机的正中心
        GetCamera("scene-main")->LookAt(title->GetPosition());

        // 设置主标题内容
        titleText->SetText(L"GameDemo");
        titleText->SetAlignMode(TextAlignMode::CenterMiddle);  // 中心位置
        titleText->SetFontSize(225);
        titleText->SetOffset({0.0f, -200.0f});
        titleText->SetTextColor(ColorRGB{"#B0C4DE"});
        // 设置副标题内容
        subheading->SetText(L"Author: QiHai");
        subheading->SetFontSize(50);
        subheading->SetOffset({0.0f, -150.0f});

        // 创建一些闪亮的东西
        // 加载效果资源
        ResourceManager::GetInstance().LoadAtlas(ASSETS_PATH "effect/bug/%d.png", 8, "bug");

        // 闪亮存在一个父对象, 利用附加的一个子对象随后clone(自动继承父对象)
        // 使用一个父对象可以将游戏对象归类
        auto bug = std::make_shared<GameObject>();
        bug->SetZOrder(RenderZOrder::UI_2);
        auto bug1 = std::make_shared<Bug>();
        bug1->Translate({0.0f, -200.0f});
        bug->AddChildObject(bug1);
        auto bug2 = bug1->Clone();
        bug2->Translate({-300.0f, 0.0f});
        auto bug3 = bug1->Clone();
        bug3->Translate({300.0f, 0.0f});

        // 添加按钮对象
        auto buttonText = std::make_shared<GameObject>(RenderZOrder::UI_1);
        auto buttonStartText = buttonText->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        buttonStartText->SetText(L"New Game");
        buttonStartText->SetAlignMode(TextAlignMode::CenterTop);
        buttonStartText->SetFontSize(60);
        buttonStartText->SetOffset({0.0f, 100.0f});
        auto buttonExitText = buttonText->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        buttonExitText->SetText(L"Exit Game");
        buttonExitText->SetAlignMode(TextAlignMode::CenterTop);
        buttonExitText->SetFontSize(60);
        buttonExitText->SetOffset({0.0f, 180.0f});

        // 添加选择动画
        ResourceManager::GetInstance().LoadAtlas(ASSETS_PATH "effect/ui_choose/%d.png", 11,
                                                 "ui_choose");
        auto ui_choose = std::make_shared<GameObject>(RenderZOrder::UI_2);
        auto ui_chooseLeft = std::make_shared<GameObject>();
        ui_chooseLeft->CreateComponent<Animator>()->AddAnimationForAtlas("idle", "ui_choose",
                                                                         false);

        ui_chooseLeft->Translate({-140.0f, 125.0f});
        ui_choose->AddChildObject(ui_chooseLeft);
        auto ui_chooseRight = ui_chooseLeft->Clone();
        ui_chooseRight->GetComponent<Animator>()->Flip();
        ui_chooseRight->Translate({280.0f, 0.0f});

        AddGameObjects({bug, title, buttonText, ui_choose});

        // 加载菜单场景的音乐
        ResourceManager::GetInstance().LoadAudio(ASSETS_PATH "audio/bgm-menu.wav", "menu-bgm");
    }

    void OnEnter() override
    {
        // 播放menu菜单音乐
        AudioManager::GetInstance().PlayAudio("menu-bgm");
    }

    void OnExit() override { AudioManager::GetInstance().StopAudio("menu-bgm"); }

private:
};