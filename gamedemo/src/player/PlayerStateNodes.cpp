#include "PlayerStateNodes.h"

#include <game_object/component/Animator.h>

PlayerStateNode::PlayerStateNode(Player* player)
    : player(player), animator(player->GetComponent<Animator>())
{
}

// === idle ===
void Idle::OnEnter()
{
    player->SetVelocity({});  // 静止
    animator->SwitchToAnimation("idle");
    animator->GetCurrentAnimation().Restart();
}
void Idle::OnUpdate()
{
    if (InputKey::TryMove()) {
        player->SwitchState("run");
    }
    if (InputKey::TryJump()) {
        player->SwitchState("jump");
    }
    if (player->GetVelocity().Y > 0) {
        player->SwitchState("fall");
    }
}

// === run ===
void Run::OnEnter() { animator->SwitchToAnimation("idleToRun"); }
void Run::OnUpdate()
{
    int dir = InputKey::GetHorizontalDir();
    player->SetVelocityX(player->xSpeed * dir);

    if (!dir) {
        player->SwitchState("runToIdle");
    }
    if (InputKey::TryJump()) {
        player->SwitchState("jump");
    }
    if (player->GetVelocity().Y > 0) {
        player->SwitchState("fall");
    }
}

// === run to idle ===
void RunToIdle::OnEnter() { animator->SwitchToAnimation("runToIdle"); }

// === jump ===
void Jump::OnEnter()
{
    animator->SwitchToAnimation("jump");
    player->SetVelocityY(-player->jumpSpeed);
}
void Jump::OnUpdate()
{
    player->SetVelocityX(player->xSpeed * InputKey::GetHorizontalDir());
    if (player->GetVelocity().Y > 0) {
        player->SwitchState("fall");
    }
}

// === fall ===
void Fall::OnEnter() { animator->SwitchToAnimation("falling"); }
void Fall::OnUpdate()
{
    player->SetVelocityX(player->xSpeed * InputKey::GetHorizontalDir());
    if (player->GetVelocity().Y == 0) {
        player->SwitchState("land");
    }
}

// === landing ===
void Land::OnEnter()
{
    player->SetVelocity({});  // 静止
    animator->SwitchToAnimation("landing");
}