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
    gameaf::log("idle");
    player->SetVelocity({});  // 静止
    animator->SwitchToAnimation("idle");
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
void Run::OnEnter()
{
    gameaf::log("run");
    animator->SwitchToAnimation("run");
}
void Run::OnUpdate()
{
    int dir = InputKey::GetHorizontalDir();
    player->SetVelocityX(player->xSpeed * dir);

    if (!dir) {
        player->SwitchState("Idle");
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

// === jump ===
void Jump::OnEnter()
{
    gameaf::log("jump");
    animator->SwitchToAnimation("jump");
    player->SetVelocityY(-player->jumpSpeed);
}
void Jump::OnUpdate()
{
    player->SetVelocityX(player->xSpeed * InputKey::GetHorizontalDir());
    if (player->GetVelocity().Y >= 0) {
        player->SwitchState("Falling");
    }
    if (InputKey::TryRoll()) {
        player->SwitchState("Roll");
    }
}

// === Falling ===
void Falling::OnEnter()
{
    gameaf::log("falling");
    animator->SwitchToAnimation("falling");
}

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
    gameaf::log("landing");
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
    gameaf::log("crouch");
    player->SetVelocity({});  // 静止
    animator->SwitchToAnimation("crouch");
    auto collisionBox = player->GetComponent<CollisionBox>();
    collisionBox->SetSize({collisionBox->GetSize().X, collisionBox->GetSize().Y - offset});
    collisionBox->SetOffset(
        {collisionBox->GetOffset().X, collisionBox->GetOffset().Y + offset / 2});
}

void Crouch::OnUpdate()
{
    if (!InputKey::TryCrouch()) {
        player->SwitchState("Idle");
    }
}

void Crouch::OnExit()
{
    auto collisionBox = player->GetComponent<CollisionBox>();
    collisionBox->SetSize({collisionBox->GetSize().X, collisionBox->GetSize().Y + offset});
    collisionBox->SetOffset(
        {collisionBox->GetOffset().X, collisionBox->GetOffset().Y - offset / 2});
}

// === roll ===
void Roll::OnEnter()
{
    gameaf::log("roll");
    player->SetVelocityX(player->dir * player->rollSpeed);
    animator->SwitchToAnimation("roll");
}

void Roll::OnUpdate()
{
    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        if (!player->isGround) {
            player->SwitchState("Falling");
        } else {
            player->SwitchState("Idle");
        }
    }
}
