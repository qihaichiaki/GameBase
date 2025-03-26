#include "PlayerStateNodes.h"

#include <GameAf.h>
#include <game_object/component/Animator.h>
#include <game_object/component/CollisionBox.h>

static GameAf& gameAf = GameAf::GetInstance();

namespace player {
PlayerStateNode::PlayerStateNode(Player* player)
    : player(player), animator(player->GetComponent<Animator>())
{
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
    player->OnJumpVfx();
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
        player->OnLandVfx();
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
    player->SetCollisonBoxOffsetY(offset);
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

void Crouch::OnExit() { player->SetCollisonBoxOffsetY(-offset); }

// === roll ===
void Roll::OnEnter()
{
    // gameaf::log("roll");
    player->SetVelocityX(player->dir * player->rollSpeed);
    animator->SwitchToAnimation("roll");
    player->SetCollisonBoxOffsetY(offset);
    player->isInvincible = true;  // 无敌时间
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

void Roll::OnExit()
{
    player->SetCollisonBoxOffsetY(-offset);
    player->isInvincible = false;
}

// === AttackStanding ===
void AttackStanding::OnEnter()
{
    // gameaf::log("AttackStanding");
    player->SetVelocityX(0.0f);
    animator->SwitchToAnimation("attack_standing");
}
void AttackStanding::OnUpdate()
{
    if (animator->GetCurrentAnimation().GetCurrentFrameIndex() == 8) {
        player->AttackStart({player->dir * 650.0f, -350.0f}, player->attackStandingDamge);
        player->isHegemonicState = true;
    }
    if (animator->GetCurrentAnimation().GetCurrentFrameIndex() == 11) {
        player->AttackEnd();
        player->isHegemonicState = false;
    }
    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        player->SwitchState("Idle");
    }
}

void AttackStanding::OnExit() { player->AttackEnd(); }

// === AttackAerial ===
void AttackAerial::OnEnter()
{
    // gameaf::log("AttackAerial");
    animator->SwitchToAnimation("attack_aerial");
    player->SetAttackBoxOffsetX(-50.0f);
    player->SetVelocity({});           // 空中静止!
    player->SetGravityEnabled(false);  // 关闭重力模拟!
}
void AttackAerial::OnUpdate()
{
    if (animator->GetCurrentAnimation().GetCurrentFrameIndex() == 9) {
        player->AttackStart({player->dir * 650.0f, 80.0f}, player->attackAirDamge);
    }
    if (animator->GetCurrentAnimation().GetCurrentFrameIndex() == 12) {
        player->AttackEnd();
    }
    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        player->SwitchState("Falling");
    }
}

void AttackAerial::OnExit()
{
    player->AttackEnd();
    player->SetAttackBoxOffsetX(50.0f);
    player->SetGravityEnabled(true);
}

// === AttackCrouching ===
void AttackCrouching::OnEnter()
{
    // gameaf::log("AttackCrouching");
    animator->SwitchToAnimation("attack_crouching");
}
void AttackCrouching::OnUpdate()
{
    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        player->SwitchState("Crouch");
    }
}

void AttackCrouching::OnExit() {}

// === block ===
void Blocking::OnEnter()
{
    // gameaf::log("block");
    player->SetVelocityX(0.0f);
    animator->SwitchToAnimation("blocking");
    player->hitAttackIntensity.X = 0.0f;
    player->isBlocking = true;
}

void Blocking::OnUpdate()
{
    if (player->hitAttackIntensity.X != 0.0f) {
        if (hitDuration == 0.0f) {
            animator->SwitchToAnimation("hitWhileBlocking");
        }
        if (hitDuration <= player->blockHitMaxDuration) {
            player->Character::SetVelocity({player->hitAttackIntensity.X * 0.15f, 0.0f});
            hitDuration += gameAf.GetDeltaTime();
        } else {
            hitDuration = 0.0f;
            player->hitAttackIntensity = {};
            player->SetVelocityX(0.0f);
            animator->SwitchToAnimation("blocking");
        }
    }
    if (!InputKey::TryBlock()) {
        player->SwitchState("Idle");
    }
}

void Blocking::OnExit()
{
    player->isBlocking = false;
    hitDuration = 0.0f;
    player->hitAttackIntensity = {};
}

// === hurt ===
void Hurt::OnEnter()
{
    player->Character::SetVelocity(player->hitAttackIntensity);
    animator->SwitchToAnimation("hurt");
}

void Hurt::OnUpdate()
{
    hurtDuration += gameAf.GetDeltaTime();
    if (hurtDuration > player->hurtMaxDuration) {
        player->SwitchState("Idle");
    }
}

void Hurt::OnExit() { hurtDuration = 0.0f; }

// === Dead ===
void Dead::OnEnter()
{
    animator->SwitchToAnimation("dead");
    player->SetVelocity({});
}
}  // namespace player