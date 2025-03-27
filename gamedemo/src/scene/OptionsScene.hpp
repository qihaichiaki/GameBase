#pragma once
#include <GameAf.h>
#include <game_object/GameObject.h>
#include <game_object/component/Text.h>
#include <scene/Scene.h>
#include <scene/SceneManager.h>

#include "../Common.hpp"

using namespace gameaf;

class OptionsScene : public Scene
{
public:
    void OnAwake() override
    {
        auto options = std::make_shared<GameObject>();
        // 左上角退出
        auto exit = options->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        exit->SetText(L"按q返回");
        exit->SetFontSize(30);
        exit->SetOffset({-GameAf::GetScreenWidth() / 2.0f, -GameAf::GetScreenHeight() / 2.0f});

        auto title = options->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        title->SetText(L"Options");
        title->SetFontSize(80);
        title->SetAlignMode(TextAlignMode::CenterTop);
        title->SetOffset({0.0f, -GameAf::GetScreenHeight() / 2.0f});

        // 具体显示信息
        auto info1 = options->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        info1->SetText(L"玩家按键映射");
        info1->SetFontSize(40);
        info1->SetOffset({-300.0f, -250.0f});

        auto info2 = options->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        info2->SetText(L"左右: A D");
        info2->SetFontSize(30);
        info2->SetOffset({-300.0f, -200.0f});

        auto info3 = options->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        info3->SetText(L"跳跃: space");
        info3->SetFontSize(30);
        info3->SetAlignMode(TextAlignMode::RightTop);
        info3->SetOffset({300.0f, -200.0f});

        auto info4 = options->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        info4->SetText(L"翻滚: LShift");
        info4->SetFontSize(30);
        info4->SetOffset({-300.0f, -160.0f});

        auto info5 = options->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        info5->SetText(L"镰刀攻击: (地面)J");
        info5->SetFontSize(30);
        info5->SetAlignMode(TextAlignMode::RightTop);
        info5->SetOffset({300.0f, -160.0f});

        auto info6 = options->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        info6->SetText(L"长剑攻击: (空中)J");
        info6->SetFontSize(30);
        info6->SetOffset({-300.0f, -120.0f});

        auto info7 = options->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        info7->SetText(L"架盾: (地面)K");
        info7->SetFontSize(30);
        info7->SetAlignMode(TextAlignMode::RightTop);
        info7->SetOffset({300.0f, -120.0f});

        AddGameObject(options);
    }

    void OnEnter() override {}

    void OnUpdate() override
    {
        if (InputKey::input.IsKeyDown(KeyValue::Q)) {
            // 返回菜单场景
            SceneManager::GetInstance().SwitchTo("menu", true);
        }
    }

    void OnExit() override {}
};