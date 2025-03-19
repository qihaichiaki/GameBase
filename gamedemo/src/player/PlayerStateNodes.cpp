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
        player->SwitchState("Run");
    }
    if (InputKey::TryJump()) {
        player->SwitchState("Jump");
    }
    if (player->GetVelocity().Y > 0) {
        player->SwitchState("Falling");
    }
    if (InputKey::TryCrouch()) {
        player->SwitchState("Crouch");
    }
}

// === run ===
void Run::OnEnter() { animator->SwitchToAnimation("idleToRun"); }
void Run::OnUpdate()
{
    int dir = InputKey::GetHorizontalDir();
    player->SetVelocityX(player->xSpeed * dir);

    if (!dir) {
        player->SwitchState("RunToIdle");
    }
    if (InputKey::TryJump()) {
        player->SwitchState("Jump");
    }
    if (player->GetVelocity().Y > 0) {
        player->SwitchState("Falling");
    }
    if (InputKey::TryCrouch()) {
        player->SwitchState("Crouch");
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
        player->SwitchState("Falling");
    }
}

// === Falling ===
void Falling::OnEnter() { animator->SwitchToAnimation("falling"); }
void Falling::OnUpdate()
{
    player->SetVelocityX(player->xSpeed * InputKey::GetHorizontalDir());
    if (player->GetVelocity().Y == 0) {
        player->SwitchState("Landing");
    }
}

// === landing ===
void Landing::OnEnter()
{
    player->SetVelocity({});  // 静止
    animator->SwitchToAnimation("landing");
}

// === crouch ===
void Crouch::OnEnter()
{
    player->SetVelocity({});  // 静止
    animator->SwitchToAnimation("crouch");
    animator->GetAnimation("crouch").Restart();
}

void Crouch::OnUpdate()
{
    if (!InputKey::TryCrouch()) {
        player->SwitchState("CrouchingToIdle");
    }
}

// === CrouchToIdle
void CrouchingToIdle::OnEnter() { animator->SwitchToAnimation("crouchingToIdle"); }