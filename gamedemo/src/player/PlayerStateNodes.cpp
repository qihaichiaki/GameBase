#include "PlayerStateNodes.h"

#include <game_object/component/Animator.h>
#include <game_object/component/CollisionBox.h>

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
    if (!player->isGround) {
        player->SwitchState("Falling");
    }
    if (InputKey::TryCrouch()) {
        player->SwitchState("Crouch");
    }
    if (InputKey::TryRoll()) {
        player->SwitchState("Roll");
    }
}

// === run ===
Run::Run(Player* player) : PlayerStateNode(player)
{
    animator->GetAnimation("idleToRun").SetOnFinished([this](Animation* animation) {
        animator->SwitchToAnimation("run");
    });
}

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
    if (!player->isGround) {
        player->SwitchState("Falling");
    }
    if (InputKey::TryCrouch()) {
        player->SwitchState("Crouch");
    }
    if (InputKey::TryRoll()) {
        player->SwitchState("Roll");
    }
}

// === run to idle ===
void RunToIdle::OnEnter() { animator->SwitchToAnimation("runToIdle"); }
void RunToIdle::OnUpdate()
{
    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        player->SwitchState("Idle");
    }
}

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
    if (InputKey::TryRoll()) {
        player->SwitchState("Roll");
    }
}

// === Falling ===
void Falling::OnEnter() { animator->SwitchToAnimation("falling"); }
void Falling::OnUpdate()
{
    player->SetVelocityX(player->xSpeed * InputKey::GetHorizontalDir());
    if (player->isGround) {
        player->SwitchState("Landing");
    }
    if (InputKey::TryRoll()) {
        player->SwitchState("Roll");
    }
}

// === landing ===
void Landing::OnEnter()
{
    player->SetVelocity({});  // 静止
    animator->SwitchToAnimation("landing");
}

void Landing::OnUpdate()
{
    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        player->SwitchState("Idle");
    }
}

// === crouch ===
void Crouch::OnEnter()
{
    player->SetVelocity({});  // 静止
    animator->SwitchToAnimation("crouch");
    animator->GetAnimation("crouch").Restart();
    auto collisionBox = player->GetComponent<CollisionBox>();
    collisionBox->SetSize({collisionBox->GetSize().X, collisionBox->GetSize().Y - offset});
    collisionBox->SetOffset(
        {collisionBox->GetOffset().X, collisionBox->GetOffset().Y + offset / 2});
}

void Crouch::OnUpdate()
{
    if (!InputKey::TryCrouch()) {
        player->SwitchState("CrouchingToIdle");
    }
}

void Crouch::OnExit()
{
    auto collisionBox = player->GetComponent<CollisionBox>();
    collisionBox->SetSize({collisionBox->GetSize().X, collisionBox->GetSize().Y + offset});
    collisionBox->SetOffset(
        {collisionBox->GetOffset().X, collisionBox->GetOffset().Y - offset / 2});
}

// === CrouchToIdle ===
void CrouchingToIdle::OnEnter() { animator->SwitchToAnimation("crouchingToIdle"); }

void CrouchingToIdle::OnUpdate()
{
    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        player->SwitchState("Idle");
    }
}

// === roll ===
void Roll::OnEnter()
{
    player->SetVelocityX(player->dir * player->rollSpeed);
    animator->SwitchToAnimation("roll");
}

void Roll::OnUpdate()
{
    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        if (!player->isGround) {
            player->SwitchState("Falling");
        } else {
            player->SwitchState("RollToIdle");
        }
    }
}

// === RollToIdle ===
void RollToIdle::OnEnter()
{
    player->SetVelocityX(InputKey::GetHorizontalDir() * player->xSpeed);
    animator->SwitchToAnimation("rollToIdle");
}

void RollToIdle::OnUpdate()
{
    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        player->SwitchState("Idle");
    }
}