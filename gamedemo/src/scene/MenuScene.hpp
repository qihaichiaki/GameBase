#pragma once

#include <game_object/GameObject.h>
#include <game_object/component/AudioManager.h>
#include <game_object/component/Text.h>
#include <game_object/widgets/ProgressBar.h>
#include <resource/ResourceManager.h>
#include <scene/Scene.h>
#include <scene/SceneManager.h>

#include <common/Log.hpp>

#include "../Common.hpp"
#include "EffectObject.hpp"
#include "UIChoose.hpp"

using namespace gameaf;

class Title : public GameObject
{
public:
    void OnAwake() override
    {
        SetName("title");
        SetZOrder(RenderZOrder::UI_1);
        titleText = CreateComponent<Text>(std::wstring{L"Syne Mono"});
        subheading = CreateComponent<Text>(std::wstring{L"Syne Mono"});

        // 设置主标题内容
        titleText->SetText(L"GameDemo");
        titleText->EnableShadow(true);
        titleText->SetAlignMode(TextAlignMode::CenterMiddle);  // 中心位置
        titleText->SetFontSize(225);
        titleText->SetOffset({0.0f, -200.0f});
        titleText->SetTextColor(ColorRGB{"#B0C4DE"});
        titleText->SetTextShadedOffset({8.0f, 8.0f});
        // 设置副标题内容
        subheading->SetText(L"Author: QiHai");
        subheading->SetFontSize(50);
        subheading->SetOffset({0.0f, -150.0f});
    }

    void OnMouseUp() override
    {
        hits++;
        if (hits == 6) {
            // 小彩蛋
            titleText->SetText(L"技术宅拯救世界");
            titleText->SetFontSize(150);  //
            subheading->SetText(L"Easter egg!");
        }
    }

private:
    int hits = 0;
    Text* titleText;
    Text* subheading;
};

class MenuScene : public Scene
{
public:
    void OnAwake() override
    {
        // 初始化时调用
        // 创建标题对象
        auto title = std::make_shared<Title>();
        auto mainCamera = GetCamera("scene-main");
        mainCamera->LookAt(title->GetPosition());  // 以0,0 设置为菜单场景的正中心

        // 创建一些闪亮的东西
        // 加载效果资源
        ResourceManager::GetInstance().LoadAtlas(ASSETS_PATH "effect/bug/%d.png", 8, "bug");

        // 闪亮存在一个父对象, 利用附加的一个子对象随后clone(自动继承父对象)
        // 使用一个父对象可以将游戏对象归类
        auto bug = std::make_shared<GameObject>();
        bug->SetZOrder(RenderZOrder::UI_2);
        auto bug1 = std::make_shared<Bug>(mainCamera.get());
        // 添加子对象
        bug->AddChildObject(bug1);
        bug1->Translate({0.0f, -200.0f});
        auto bug2 = bug1->Clone();
        bug2->Translate({-300.0f, 0.0f});
        auto bug3 = bug1->Clone();
        bug3->Translate({300.0f, 0.0f});
        bug->SetActive(false);
        bug->SetChildrenActive(true);

        // 添加按钮对象
        ResourceManager::GetInstance().LoadAtlas(ASSETS_PATH "effect/ui_choose/%d.png", 11,
                                                 "ui_choose");
        auto buttonNew = std::make_shared<UIChoose>(L"New Game");
        auto buttonExit = std::make_shared<UIChoose>(L"Exit Game");

        buttonNew->Translate({0.0f, 140.0f});
        buttonExit->Translate({0.0f, 220.0f});

        AddGameObjects({bug, title, buttonNew, buttonExit});

        buttonNew->RegisterMouseClicked(
            []() { SceneManager::GetInstance().SwitchTo("game", true); });
        buttonExit->RegisterMouseClicked([]() { GameAF::GetInstance().Exit(); });  // 设置游戏退出
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