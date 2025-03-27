#pragma once
#include <GameAf.h>
#include <game_object/GameObject.h>
#include <game_object/component/Image.h>
#include <game_object/component/Text.h>
#include <scene/Scene.h>
#include <scene/SceneManager.h>

#include "../Common.hpp"

using namespace gameaf;

class AboutScene : public Scene
{
public:
    void OnAwake() override
    {
        auto about = std::make_shared<GameObject>();
        // 左上角退出
        auto exit = about->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        exit->SetText(L"按q返回");
        exit->SetFontSize(30);
        exit->SetOffset({-GameAf::GetScreenWidth() / 2.0f, -GameAf::GetScreenHeight() / 2.0f});

        about->CreateComponent<Image>(std::string{"test"})->SetOffset({0.0f, 50.0f});

        auto title = about->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        title->SetText(L"About");
        title->SetFontSize(80);
        title->SetAlignMode(TextAlignMode::CenterTop);
        title->SetOffset({0.0f, -GameAf::GetScreenHeight() / 2.0f});

        // 具体显示信息
        auto info1 = about->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        info1->SetText(
            L"大家好, "
            L"我是qihai\n本游戏是基于我个人理解和实践,利用C++17+easyx+"
            L"MCI等实现的一个简单2D游戏引擎\n当前阶段音频似乎影响很大\n本游戏当前关卡是基于强化学习"
            L"后的AI大黄蜂, 玩家和她进行对抗\n本游戏仅用于交流学习, "
            L"谢谢!\n项目链接:https://github.com/qihaichiaki/GameBase");
        info1->SetAlignMode(TextAlignMode::CenterTop);
        info1->EnableShadow(true);
        info1->SetTextShadedColor(ColorRGB{0, 0, 0});
        info1->SetFontSize(32);
        info1->SetOffset({0.0f, -250.0f});

        auto info2 = about->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        info2->SetText(
            L"借物表\n"
            L"参考游戏素材: 空洞骑士(HollowKnight)、lostruins\n"
            L"素材来源\n"
            L"https://www.spriters-resource.com\n"
            L"游戏解包/网上寻找等\n");
        info2->SetAlignMode(TextAlignMode::CenterTop);
        info2->EnableShadow(true);
        info2->SetTextShadedColor(ColorRGB{0, 0, 0});
        info2->SetFontSize(32);
        info2->SetOffset({0.0f, 100.0f});

        AddGameObject(about);
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