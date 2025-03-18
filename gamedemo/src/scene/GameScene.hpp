#pragma once

#include <GameAf.h>
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
        auto uiCamera = std::make_shared<Camera>(Vector2{
            GameAf::GetScreenWidth() * 1.0f,
            GameAf::GetScreenHeight() * 1.0f,
        });                             // 创建ui摄像机, 大小为当前屏幕大小
        uiCamera->SetRenderMod(false);  // UI相机启动非全局渲染模式, 即必须指定游戏对象才能渲染
        AddCamera("ui", uiCamera);      // 添加到游戏场景内

        // 添加背景对象
        auto backgroundBottom = std::make_shared<BackgroundBottom>();
        uiCamera->AddRenderObj("BackgroundBottom");        // UI渲染
        mainCamera->DisableRenderObj("BackgroundBottom");  // main相机不渲染

        // 添加player对象
        auto player = std::make_shared<Player>();

        // 创建中间层背景
        auto backgroundMiddle = std::make_shared<BackgroundMiddle>(player.get());  // 世界坐标渲染
        // 创建地板
        auto ground = std::make_shared<Ground>();

        // 主相机跟随玩家
        mainCamera->SetFollowTarget(player, Camera::FollowMode::Smooth);
        mainCamera->SetCameraDeadZone(Vector2{200.0f, 600.0f});
        SetDebugRenderCamera(true);

        AddGameObjects({backgroundBottom, backgroundMiddle, ground, player});
    }

    void OnEnter() override
    {
        Player* player = static_cast<Player*>(GetGameObject("player").get());
        player->SetPosition({});  // 玩家位置复原
        player->SetVelocity({});
        GetCamera("main")->LookAt({});  // 相机位置复原
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