#pragma once

#include <GameAf.h>
#include <game_object/GameObject.h>
#include <game_object/component/Image.h>
#include <game_object/component/Text.h>
#include <input/InputManager.h>
#include <resource/ResourceManager.h>
#include <scene/Scene.h>
#include <scene/SceneManager.h>

#include <common/Log.hpp>

#include "../Common.hpp"
#include "../player/Player.hpp"

using namespace gameaf;

class GameScene : public Scene
{
public:
    void OnAwake() override
    {
        auto test = std::make_shared<GameObject>(RenderZOrder::BACKGROUND, "TestBackground");
        // 简单设置文字
        auto testText1 = test->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        testText1->SetText(L"Intense game scenes");
        testText1->SetFontSize(80);
        testText1->SetTextBoxSize(
            {GameAF::GetScreenWidth() * 1.0f, GameAF::GetScreenHeight() * 1.0f});
        testText1->SetAlignMode(TextAlignMode::CenterMiddle);
        auto testText2 = test->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        testText2->SetText(L"Press q to return");
        testText2->SetFontSize(30);
        testText2->SetOffset({100.0f, testText1->GetRenderBoxSize().Y / 2});

        // 简单设置背景
        auto img = test->CreateComponent<Image>(std::string{"test"});
        img->SetOffset({0.0f, 180.0f});

        // 添加player资源
        auto player = std::make_shared<Player>();
        player->Translate({0.0f, 200.0f});

        AddGameObjects({test, player});
    }

    void OnUpdate() override
    {
        static auto& input = InputManager::GetInstance();

        if (input.IsKeyDown(InputManager::KeyValue::Q)) {
            // 返回菜单场景
            SceneManager::GetInstance().SwitchTo("menu", true);
        }
    }

private:
};