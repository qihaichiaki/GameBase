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

    if (hornet->GetVelocity().Y > 0) {
        hornet->SwitchState("Fall");
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
    hornet->OnJumpVfx();
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
    // 超出地图底下了
    if (hornet->GetPosition().Y >= 340.0f) {
        // 猜测只有hurt的时候被这样搞
        hornet->SwitchState("RushJump");
    }

    if (hornet->isGround) {
        hornet->SwitchState("Idle");
        hornet->OnLandVfx();
    }
}

// === dash ===
Dash::Dash(Hornet* hornet) : HornetStateNode(hornet) {}

void Dash::OnEnter()
{
    // gameaf::log("dash dir: {}", hornet->dir);
    animator->SwitchToAnimation("dash");
    hornet->SetVelocityX(hornet->dir * hornet->dashSpeed);
    hornet->isInvincible = true;
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

void Dash::OnExit() { hornet->isInvincible = false; }

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
        int chance = gameAf.Random(0, 100);
        if (chance <= 50) {
            hornet->SwitchState("Idle");
        } else if (chance <= 65) {
            hornet->Flip();
            hornet->SwitchState("Jump");
        } else {
            hornet->Flip();
            hornet->SwitchState("Dash");
        }
    }
    if (walkDuration >= currentWalkMaxDuration) {
        hornet->SwitchState("Idle");
    }
}

// === Hurt ===
Hurt::Hurt(Hornet* hornet) : HornetStateNode(hornet) {}

void Hurt::OnEnter()
{
    hornet->SetCollisonBoxOffsetY(100.0f);
    hornet->SetVelocity(hornet->hurtAttackIntensity);
    animator->SwitchToAnimation("hurt");
}
void Hurt::OnUpdate()
{
    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        hornet->SwitchState("Fall");
    }
}

void Hurt::OnExit() { hornet->SetCollisonBoxOffsetY(-100.0f); }

// === RushJump ===
RushJump::RushJump(Hornet* hornet) : HornetStateNode(hornet) {}

void RushJump::OnEnter()
{
    gameaf::log("开始超级跳");
    animator->SwitchToAnimation("rushJump");
    jumpXSpeed = 0.0f;
}
void RushJump::OnUpdate()
{
    hornet->SetVelocityX(jumpXSpeed * hornet->dir);
    if (animator->GetCurrentAnimation().GetCurrentFrameIndex() < 2) {
        hornet->SetVelocityY(0.0f);
    }
    if (animator->GetCurrentAnimation().GetCurrentFrameIndex() == 1) {
        // 开始起飞
        jumpXSpeed = gameAf.Random(hornet->minRushJumpXSpeed, hornet->maxRushJumpXSpeed);
        hornet->SetVelocityY(-gameAf.Random(hornet->minRushJumpYSpeed, hornet->maxRushJumpYSpeed));
    }
    if (animator->GetCurrentAnimation().IsEndOfPlay() && hornet->GetVelocity().Y >= 0) {
        hornet->SwitchState("Fall");
    }
}

}  // namespace hornet