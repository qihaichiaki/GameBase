#include "HornetStateNodes.h"

#include <GameAf.h>
#include <game_object/component/Animator.h>

#include "../player/Player.h"

static GameAf& gameAf = GameAf::GetInstance();

namespace hornet {
HornetStateNode::HornetStateNode(Hornet* hornet)
    : hornet(hornet), animator(hornet->GetComponent<Animator>())
{
}

// === sit ===
void Sit::OnEnter() { animator->SwitchToAnimation("sit"); }

void Sit::OnUpdate()
{
    if (hornet->PlayerInRange(hornet->searchPlayerRange)) {
        hornet->SwitchState("Idle");
    }
}

// === Idle ===
Idle::Idle(Hornet* hornet) : HornetStateNode(hornet) {}

void Idle::OnEnter()
{
    // gameaf::log("idle dir: {}", hornet->dir);
    animator->SwitchToAnimation("idle");
    hornet->SetVelocityX(0.0f);
    curentPlayerInEvadeMaxDuration =
        gameAf.Random(hornet->playerInEvadeMinDuration, hornet->playerInEvadeMaxDuration);
}

void Idle::OnUpdate()
{
    hornet->OrientationPlayer();  // 保持朝向玩家
    if (hornet->PlayerInRange(hornet->evadeRange)) {
        if (playerInEvadeDuration >= curentPlayerInEvadeMaxDuration) {
            int chance = gameAf.Random(0, 100);
            if (chance <= 45) {
                // 45% 躲闪
                hornet->SwitchState("Evade");
            } else if (chance <= 70) {
                // 25% dash
                hornet->SwitchState("Dash");
            } else if (chance <= 90) {
                // 20% walk
                hornet->Flip();
                hornet->SwitchState("Walk");

            } else {
                // 10% 什么也不做
                playerInEvadeDuration = 0.0f;
            }
        }
        playerInEvadeDuration += gameAf.GetDeltaTime();
    } else {
        playerInEvadeDuration = 0.0f;  // 重置
    }
}

void Idle::OnExit() { playerInEvadeDuration = 0.0f; }

// === evade ===
Evade::Evade(Hornet* hornet) : HornetStateNode(hornet) {}

void Evade::OnEnter()
{
    animator->SwitchToAnimation("evade");
    hornet->SetVelocityX(-hornet->dir * hornet->evadeSpeed);
}

void Evade::OnUpdate()
{
    if (!hornet->isGround || hornet->GetVelocity().X == 0) {
        // 如果此时闪避空了或者闪避到墙了, 跳跃回去
        hornet->SwitchState("Jump");
    }
    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        hornet->SwitchState("Idle");
    }
}

// === Jump ===
Jump::Jump(Hornet* hornet) : HornetStateNode(hornet) {}

void Jump::OnEnter()
{
    // gameaf::log("jump dir: {}", hornet->dir);
    animator->SwitchToAnimation("jump");
    jumpXSpeed = gameAf.Random(0.0f, hornet->maxJumpXSpeed);
    hornet->SetVelocity(
        Vector2{hornet->dir * jumpXSpeed,
                -1.0f * gameAf.Random(hornet->minJumpYSpeed, hornet->maxJumpYSpeed)});
}

void Jump::OnUpdate()
{
    hornet->SetVelocityX(hornet->dir * jumpXSpeed);
    if (hornet->GetVelocity().Y >= 0) {
        hornet->SwitchState("Fall");
    }
}

// === Fall ===
Fall::Fall(Hornet* hornet) : HornetStateNode(hornet) {}

void Fall::OnEnter() { animator->SwitchToAnimation("fall"); }

void Fall::OnUpdate()
{
    if (hornet->isGround) {
        hornet->SwitchState("Idle");
    }
}

// === dash ===
Dash::Dash(Hornet* hornet) : HornetStateNode(hornet) {}

void Dash::OnEnter()
{
    // gameaf::log("dash dir: {}", hornet->dir);
    animator->SwitchToAnimation("dash");
    hornet->SetVelocityX(hornet->dir * hornet->dashSpeed);
}

void Dash::OnUpdate()
{
    // 冲出头了跳回来
    if (!hornet->isGround) {
        hornet->Flip();
        hornet->SwitchState("Jump");
    }
    if (hornet->GetVelocity().X == 0.0f) {  // 碰到墙停下
        int chance = gameAf.Random(0, 100);
        if (chance <= 50) {
            hornet->SwitchState("Idle");
        } else {
            hornet->SwitchState("Jump");
        }
    }

    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        if (hornet->isGround) {
            hornet->SwitchState("Idle");
        } else {
            hornet->SwitchState("Fall");
        }
    }
}

// === Walk ===
Walk::Walk(Hornet* hornet) : HornetStateNode(hornet) {}

void Walk::OnEnter()
{
    // gameaf::log("walk dir {}", hornet->dir);
    animator->SwitchToAnimation("walk");
    hornet->SetVelocityX(hornet->dir * hornet->walkSpeed);
    walkDuration = 0.0f;
    currentWalkMaxDuration = gameAf.Random(hornet->walkMinDuration, hornet->walkMaxDuration);
}

void Walk::OnUpdate()
{
    walkDuration += gameAf.GetDeltaTime();
    // 如果即将坠落
    if (hornet->isAboutToFall || hornet->GetVelocity().X == 0.0f) {
        hornet->Flip();

        if (gameAf.Random(0, 100) <= 50) {
            hornet->SwitchState("Jump");
        } else {
            hornet->SwitchState("Dash");
        }
    }
    if (walkDuration >= currentWalkMaxDuration) {
        hornet->SwitchState("Idle");
    }
}

}  // namespace hornet