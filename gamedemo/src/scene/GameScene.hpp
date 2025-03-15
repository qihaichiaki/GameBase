#pragma once

#include <GameAf.h>
#include <game_object/GameObject.h>
#include <game_object/component/Image.h>
#include <game_object/component/Text.h>
#include <input/InputManager.h>
#include <resource/ResourceManager.h>
#include <scene/Scene.h>
#include <scene/SceneManager.h>

using namespace gameaf;

class GameScene : public Scene
{
public:
    void OnAwake() override
    {
        auto& gameaf = GameAF::GetInstance();

        auto test = std::make_shared<GameObject>();
        // 简单设置文字
        auto testText1 = test->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        testText1->SetText(L"Intense game scenes");
        testText1->SetFontSize(80);
        testText1->SetTextBoxSize({gameaf.GetScreenWidth(), gameaf.GetScreenHeight()});
        testText1->SetAlignMode(TextAlignMode::CenterMiddle);
        auto testText2 = test->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        testText2->SetText(L"Press q to return");
        testText2->SetFontSize(30);
        testText2->SetOffset({100.0f, testText1->GetRenderBoxSize().Y / 2});

        // 简单设置背景
        // 加载背景
        ResourceManager::GetInstance().NewImage(ASSETS_PATH "background/test.jpg", "test");
        auto img = test->CreateComponent<Image>(std::string{"test"});
        img->SetOffset({0.0f, 180.0f});

        AddGameObject(test);
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