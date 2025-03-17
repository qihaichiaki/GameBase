#pragma once

#include <game_object/GameObject.h>
#include <game_object/component/Animator.h>
#include <game_object/component/CollisionBox.h>
#include <game_object/component/Image.h>
#include <game_object/component/Text.h>
#include <input/InputManager.h>

#include <common/Log.hpp>

#include "../Common.hpp"

using namespace gameaf;

class Player : public GameObject
{
public:
    Player() : GameObject(RenderZOrder::PLAYER, "player") {}

    void OnAwake() override
    {
        // 加载角色动画
        animator = CreateComponent<Animator>();
        // 精灵图中 0~5 为idle动画
        animator->AddAnimationForImage("idle", "player", 0, 6, true);
        animator->AddAnimationForImage("run", "player", 6, 8, true);
        animator->AddAnimationForImage("attack1", "player", 14, 7, true);  // 攻击1段
        animator->AddAnimationForImage("attack2", "player", 21, 5, true);  // 攻击2段
        animator->AddAnimationForImage("attack3", "player", 76, 8, true);  // 攻击3段
        animator->AddAnimationForImage("dead", "player", 26, 11, true);
        animator->AddAnimationForImage("hit", "player", 37, 4, true);
        animator->AddAnimationForImage("jump", "player", 41, 4, false);
        animator->AddAnimationForImage("fall", "player", 45, 4, false);
        animator->AddAnimationForImage("hang", "player", 49, 11, true);       // 悬挂
        animator->AddAnimationForImage("wall_slide", "player", 60, 3, true);  // 滑墙
        animator->AddAnimationForImage("duck", "player", 63, 5, true);        // 下蹲
        animator->AddAnimationForImage("dash", "player", 69, 7, true);        // 冲刺
        animator->AddAnimationForImage("slide", "player", 84, 7, true);       // 滑铲
        animator->AddAnimationForImage("wall_climb", "player", 91, 8, true);  // 爬墙

        animator->SetSizeScale({2.5f, 2.5f});
    }

    void OnUpdate() override {}

    void OnDraw(const Camera& camera) override {}

public:
    GameObjectPtr Clone() override
    {
        auto clonePtr = std::make_shared<Player>(*this);
        if (auto parent = GetParent()) {
            parent->AddChildObject(clonePtr);
        }
        return clonePtr;
    }

private:
    Animator* animator;
};

// 测试动画使用
void TestPlayerAnimator(Animator* animator, Text* text)
{
    static auto& input = InputManager::GetInstance();

    // 测试角色动画
    if (input.GetKey(InputManager::KeyValue::Shift)) {
        if (input.IsKeyDown(InputManager::KeyValue::_1)) {
            animator->SwitchToAnimation("wall_slide");
            text->SetText(L"wall_slide");
        }
        if (input.IsKeyDown(InputManager::KeyValue::_2)) {
            animator->SwitchToAnimation("duck");
            text->SetText(L"duck");
        }
        if (input.IsKeyDown(InputManager::KeyValue::_3)) {
            animator->SwitchToAnimation("dash");
            text->SetText(L"dash");
        }
        if (input.IsKeyDown(InputManager::KeyValue::_4)) {
            animator->SwitchToAnimation("slide");
            text->SetText(L"slide");
        }
        if (input.IsKeyDown(InputManager::KeyValue::_5)) {
            animator->SwitchToAnimation("wall_climb");
            text->SetText(L"wall_climb");
        }
    } else {
        if (input.IsKeyDown(InputManager::KeyValue::_1)) {
            animator->SwitchToAnimation("idle");
            text->SetText(L"idle");
        }
        if (input.IsKeyDown(InputManager::KeyValue::_2)) {
            animator->SwitchToAnimation("run");
            text->SetText(L"run");
        }
        if (input.IsKeyDown(InputManager::KeyValue::_3)) {
            animator->SwitchToAnimation("attack1");
            text->SetText(L"attack1");
        }
        if (input.IsKeyDown(InputManager::KeyValue::_4)) {
            animator->SwitchToAnimation("attack2");
            text->SetText(L"attack2");
        }
        if (input.IsKeyDown(InputManager::KeyValue::_5)) {
            animator->SwitchToAnimation("attack3");
            text->SetText(L"attack3");
        }
        if (input.IsKeyDown(InputManager::KeyValue::_6)) {
            animator->SwitchToAnimation("dead");
            text->SetText(L"dead");
        }
        if (input.IsKeyDown(InputManager::KeyValue::_7)) {
            animator->SwitchToAnimation("hit");
            text->SetText(L"hit");
        }
        if (input.IsKeyDown(InputManager::KeyValue::_8)) {
            animator->SwitchToAnimation("jump");
            text->SetText(L"jump");
        }
        if (input.IsKeyDown(InputManager::KeyValue::_9)) {
            animator->SwitchToAnimation("fall");
            text->SetText(L"fall");
        }
        if (input.IsKeyDown(InputManager::KeyValue::_0)) {
            animator->SwitchToAnimation("hang");
            text->SetText(L"hang");
        }
    }
}