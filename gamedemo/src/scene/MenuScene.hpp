#pragma once

#include <game_object/GameObject.h>
#include <game_object/component/Text.h>
#include <scene/Scene.h>

#include <common/Log.hpp>

#include "../Common.hpp"

using namespace gameaf;

class MenuScene : public Scene
{
public:
    void OnAwake() override
    {
        // 初始化时调用
        // 创建标题对象
        auto title = std::make_shared<GameObject>(RenderZOrder::UI, "title");
        auto titleText = title->CreateComponent<Text>(std::wstring{L"Syne Mono"});
        // 设置到场景主相机的正中心
        GetCamera("scene-main")->LookAt(title->GetPosition());

        // 设置字体内容
        titleText->SetText(L"GameDemo");
        titleText->SetAlignMode(TextAlignMode::CenterMiddle);  // 中心位置
        titleText->SetFontSize(225);
        titleText->SetOffset({0.0f, -200.0f});

        AddGameObject(title);
    }

private:
};