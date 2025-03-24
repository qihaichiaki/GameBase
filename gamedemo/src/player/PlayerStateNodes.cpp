#include "PlayerStateNodes.h"

#include <GameAf.h>
#include <game_object/component/Animator.h>
#include <game_object/component/CollisionBox.h>

namespace player {
PlayerStateNode::PlayerStateNode(Player* player)
    : player(player), animator(player->GetComponent<Animator>())
{
}

void PlayerStateNode::SetSizeY(float offsetY)
{
    auto collisionBox = player->GetComponent<CollisionBox>();
    collisionBox->SetSize({collisionBox->GetSize().X, collisionBox->GetSize().Y - offsetY});
    collisionBox->SetOffset(
        {collisionBox->GetOffset().X, collisionBox->GetOffset().Y + offsetY / 2});
}

// === idle ===
void Idle::OnEnter()
{
    // gameaf::log("idle");
    player->SetVelocity({});  // 静止
    animator->SwitchToAnimation("idle");
    player->attackAerialNum = player->maxAttackAerialNum;  // 恢复空中攻击数
    player->jumpNum = player->maxjumpNum;                  // 恢复空中跳跃数
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
    if (InputKey::TryAttack()) {
        player->SwitchState("AttackStanding");
    }
    if (InputKey::TryBlock()) {
        player->SwitchState("Blocking");
    }
}

// === run ===
void Run::OnEnter()
{
    // gameaf::log("run");
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
    // 针对落下时, 添加计时器, 跳跃宽限期, 方便玩家跳跃
    if (!player->isGround) {
        player->SwitchState("Falling");
        player->isOnFloorLastFrameTimer.Restart();  // 跳跃宽限
        player->isJumpForgiveExit = true;
    }
    if (InputKey::TryCrouch()) {
        player->SwitchState("Crouch");
    }
    if (InputKey::TryRoll()) {
        player->SwitchState("Roll");
    }
    if (InputKey::TryAttack()) {
        player->SwitchState("AttackStanding");
    }
    if (InputKey::TryBlock()) {
        player->SwitchState("Blocking");
    }
}

// === jump ===
void Jump::OnEnter()
{
    // gameaf::log("jump");
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
    if (InputKey::TryJump() && player->jumpNum > 0) {
        player->jumpNum--;
        player->SwitchState("Jump");
    }
    if (InputKey::TryAttack() && player->attackAerialNum > 0) {
        player->attackAerialNum--;
        player->SwitchState("AttackAerial");
    }
}

// === Falling ===
void Falling::OnEnter()
{
    // gameaf::log("falling");
    animator->SwitchToAnimation("falling");
}

void Falling::OnUpdate()
{
    player->SetVelocityX(player->xSpeed * InputKey::GetHorizontalDir());

    player->isOnFloorLastFrameTimer.OnUpdate(
        GameAf::GetInstance().GetDeltaTime());  // 跳跃宽限定时器更新
    if (InputKey::TryJump()) {
        if (player->isJumpForgiveExit) {
            player->SwitchState("Jump");
        } else if (player->jumpNum > 0) {
            player->jumpNum--;
            player->SwitchState("Jump");
        }
    }

    if (player->isGround) {
        player->SwitchState("Idle");
    }
    if (InputKey::TryRoll()) {
        player->SwitchState("Roll");
    }
    if (InputKey::TryAttack() && player->attackAerialNum > 0) {
        player->attackAerialNum--;
        player->SwitchState("AttackAerial");
    }
}

// === crouch ===
void Crouch::OnEnter()
{
    // gameaf::log("crouch");
    player->SetVelocity({});  // 静止
    animator->SwitchToAnimation("crouch");
    SetSizeY(offset);
}

void Crouch::OnUpdate()
{
    if (!InputKey::TryCrouch()) {
        player->SwitchState("Idle");
    }
    if (InputKey::TryRoll()) {
        player->SwitchState("Roll");
    }
    if (InputKey::TryAttack()) {
        player->SwitchState("AttackCrouching");
    }
}

void Crouch::OnExit() { SetSizeY(-offset); }

// === roll ===
void Roll::OnEnter()
{
    // gameaf::log("roll");
    player->SetVelocityX(player->dir * player->rollSpeed);
    animator->SwitchToAnimation("roll");
    SetSizeY(offset);
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

void Roll::OnExit() { SetSizeY(-offset); }

void Attack::OnEnter() {}

// === AttackStanding ===
void AttackStanding::OnEnter()
{
    Attack::OnEnter();
    // gameaf::log("AttackStanding");
    player->SetVelocityX(0.0f);
    animator->SwitchToAnimation("attack_standing");
}
void AttackStanding::OnUpdate()
{
    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        player->SwitchState("Idle");
    }
}

// === AttackAerial ===
void AttackAerial::OnEnter()
{
    Attack::OnEnter();
    // gameaf::log("AttackAerial");
    animator->SwitchToAnimation("attack_aerial");
}
void AttackAerial::OnUpdate()
{
    player->SetVelocity({});  // 空中静止!
    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        player->SwitchState("Falling");
    }
}

// === AttackCrouching ===
void AttackCrouching::OnEnter()
{
    Attack::OnEnter();
    // gameaf::log("AttackCrouching");
    animator->SwitchToAnimation("attack_crouching");
}
void AttackCrouching::OnUpdate()
{
    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        player->SwitchState("Crouch");
    }
}

// === block ===
void Blocking::OnEnter()
{
    // gameaf::log("block");
    player->SetVelocityX(0.0f);
    animator->SwitchToAnimation("blocking");
}

void Blocking::OnUpdate()
{
    if (!InputKey::TryBlock()) {
        player->SwitchState("Idle");
    }
}
}  // namespace player