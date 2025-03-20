#pragma once

#include <GameAf.h>
#include <game_object/component/Text.h>
#include <input/InputManager.h>
#include <resource/ResourceManager.h>
#include <scene/Camera.h>
#include <scene/Scene.h>
#include <scene/SceneManager.h>

#include <common/Log.hpp>

#include "../Common.hpp"
#include "../other_object/Background.hpp"
#include "../player/Player.h"

using namespace gameaf;

class GameScene : public Scene
{
public:
    void OnAwake() override
    {
        auto mainCamera = GetCamera("main").get();  // 默认主相机, 全局渲染模式
        Vector2 screenSize = {GameAf::GetScreenWidth() * 1.0f, GameAf::GetScreenHeight() * 1.0f};
        auto uiCamera = std::make_shared<Camera>(screenSize);  // 创建ui摄像机, 大小为当前屏幕大小
        uiCamera->SetRenderMod(false);  // UI相机启动非全局渲染模式, 即必须指定游戏对象才能渲染
        AddCamera("ui", uiCamera);      // 添加到游戏场景内

        // 添加背景对象
        auto backgroundBottom = std::make_shared<BackgroundBottom>();
        uiCamera->AddRenderObj("BackgroundBottom");        // UI渲染
        mainCamera->DisableRenderObj("BackgroundBottom");  // main相机不渲染

        // 添加测试提示文本
        auto test = std::make_shared<GameObject>("TestText");
        test->SetPosition(screenSize / 2.0f);
        _testText = test->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        _testText->SetAlignMode(TextAlignMode::RightBottom);
        _testText->SetTextColor(ColorRGB{255, 0, 0});
        uiCamera->AddRenderObj("TestText");        // UI渲染
        mainCamera->DisableRenderObj("TestText");  // main相机不渲染

        // 添加player对象
        auto player = std::make_shared<Player>();
        _player = player.get();

        // 创建中间层背景
        auto backgroundMiddle = std::make_shared<BackgroundMiddle>(player.get());  // 世界坐标渲染
        // 创建地板
        auto ground = std::make_shared<Ground>();
        // 创建墙面
        auto wall = std::make_shared<Wall>();
        wall->Translate({0.0f, 150.0f});

        AddGameObjects({test, backgroundBottom, backgroundMiddle, ground, wall, player});

        // 主相机跟随玩家
        mainCamera->SetFollowTarget(player, Camera::FollowMode::Smooth);
        mainCamera->SetCameraDeadZone(Vector2{200.0f, 700.0f});
        mainCamera->SetFixdDeadZone(Vector2{}, ground->GetComponent<Image>()->GetSize() + 100.0f);
    }

    void OnEnter() override
    {
        _player->ReStart();
        GetCamera("main")->LookAt({});  // 相机位置复原
    }

    void OnUpdate() override
    {
        if (InputKey::input.IsKeyDown(KeyValue::Q)) {
            // 返回菜单场景
            SceneManager::GetInstance().SwitchTo("menu", true);
        }

        if (InputKey::input.IsKeyDown(KeyValue::P)) {
            // 进入开发者模式 debug
            bool isDebug = !_player->isDebug;
            _player->isDebug = isDebug;
            SetDebugRenderCamera(isDebug);
            static_cast<Ground*>(GetGameObject("Ground").get())->isDebug = isDebug;
            static_cast<Wall*>(GetGameObject("Wall").get())->isDebug = isDebug;
            if (isDebug) {
                _testText->SetText(L"调试模式");
            } else {
                _testText->SetText(L"");
            }
        }

        // 检查玩家位置用于修正
        if (_player->GetPosition().Y >= 1000.0f) {
            _player->ReStart();
        }
    }

private:
    Player* _player;
    Text* _testText;
};