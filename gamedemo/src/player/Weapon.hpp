#pragma once

#include <GameAf.h>
#include <game_object/GameObject.h>
#include <game_object/component/Image.h>

#include <game_object/component/Timer.hpp>

#include "../Common.hpp"

static constexpr double PI = 3.1415926535;
static constexpr double MAX_RADIAN = (PI * 2.0f / 3.0f);

static constexpr double STEPS = (2.0 * MAX_RADIAN) / 19.0f;

/**
 * 玩家武器对象
 * 废弃方案, 暂时没有使用
 */

using namespace gameaf;

class Weapon : public GameObject
{
public:
    Weapon() : GameObject(RenderZOrder::PLAYER, "Weapon") {}

    void OnAwake() override
    {
        img = CreateComponent<Image>(std::string{"sickleOfDeath"});
        img->SetSizeScale({3.0f, 3.0f});
        img->Rotate(radian);
        img->SetOffset({20.0f, -100.0f});

        rotateTimer.SetWaitTime(0.06f);
        rotateTimer.SetOnTimeout([&]() {
            radian -= STEPS;
            if (radian <= -MAX_RADIAN) {
                SetActive(false);
                radian = MAX_RADIAN;
            }
            img->Rotate(radian);  // 旋转
        });
        SetActive(false);
    }

    void OnUpdate() override { rotateTimer.OnUpdate(GameAf::GetInstance().GetDeltaTime()); }

public:
    void Attack()
    {
        SetActive(true);
        rotateTimer.Restart();
    }

private:
    Image* img;
    double radian = MAX_RADIAN;
    Timer rotateTimer;
};