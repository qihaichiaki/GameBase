#include "HornetStateNodes.h"

#include <GameAf.h>
#include <game_object/component/Animator.h>
#include <game_object/component/AudioManager.h>

#include "../rl/LoadModel.h"

static GameAf& gameAf = GameAf::GetInstance();

namespace hornet {
HornetStateNode::HornetStateNode(Hornet* hornet)
    : hornet(hornet), animator(hornet->GetComponent<Animator>())
{
}

void HornetStateNode::OnEnter()
{
    currentDuration = 0.0f;  // 重置当前持续时间
    // 当前状态下的决策时间
    decisionMakingTime =
        gameAf.Random(hornet->minDecisionMakingTime, hornet->maxDecisionMakingTime);
}

void HornetStateNode::OnUpdate()
{
    currentDuration += gameAf.GetDeltaTime();
    if (currentDuration >= decisionMakingTime) {
        ExecutiveDecision();  // 尝试执行决策
        currentDuration = 0.0f;
    }
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
    HornetStateNode::OnEnter();
    animator->SwitchToAnimation("idle");
    hornet->SetVelocityX(0.0f);
    hornet->currentAction = (int)HornetAction::Idle;
}

void Idle::OnUpdate()
{
    hornet->OrientationPlayer();  // 保持朝向玩家
    if (hornet->GetVelocity().Y > 0) {
        hornet->SwitchState("Fall");
    }
    HornetStateNode::OnUpdate();
}

void Idle::ExecutiveDecision()
{
    if (hornet->isdqnAi) {
        ScriptModule::PredictAction(hornet, hornet->dstEnemy);
    } else {
        if (hornet->PlayerInRange(hornet->evadeRange)) {
            int chance = gameAf.Random(0, 100);
            // 对于敌对状态下, 这种情况下, 可能会进行近战攻击
            if (hornet->isHostileState && gameAf.Random(0, 100) <= 50) {
                // 攻击状态
                if (chance <= 65) {
                    if (hornet->CanAttackHorizontal()) {
                        hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy,
                                                            HornetAction::Attack, hornet->reward);
                        hornet->reward = 0.0f;
                        hornet->SwitchState("Attack");
                    } else {
                        hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy,
                                                            HornetAction::AttackUp, hornet->reward);
                        hornet->reward = 0.0f;
                        hornet->SwitchState("AttackUp");
                    }
                } else {
                    hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy,
                                                        HornetAction::Defend, hornet->reward);
                    hornet->reward = 0.0f;
                    hornet->SwitchState("Defend");  // 尝试格挡
                }
            } else {
                // 躲闪状态
                if (chance <= 45) {
                    // 45% 躲闪
                    hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy,
                                                        HornetAction::Evade, hornet->reward);
                    hornet->reward = 0.0f;
                    hornet->SwitchState("Evade");
                } else if (chance <= 70) {
                    // 25% dash
                    hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy,
                                                        HornetAction::Dash, hornet->reward);
                    hornet->reward = 0.0f;
                    hornet->SwitchState("Dash");
                } else if (chance <= 90) {
                    // 20% walk
                    hornet->Flip();
                    hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy,
                                                        HornetAction::Walk, hornet->reward);
                    hornet->reward = 0.0f;
                    hornet->SwitchState("Walk");
                } else {
                    // 10% 什么也不做
                    hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy,
                                                        HornetAction::DoNothing, hornet->reward);
                    hornet->reward = -0.5f;
                }
            }
        } else {
            // 存在仇恨的情况下 玩家没有在近战攻击范围内, 但是还是在索敌范围内
            if (hornet->isHostileState && hornet->PlayerInRange(hornet->searchPlayerRange)) {
                int chance = gameAf.Random(0, 100);

                if (chance <= 40) {
                    if (hornet->CanAttackHorizontal()) {
                        hornet->trainingData.RecordDecision(
                            hornet, hornet->dstEnemy, HornetAction::DashAttackAim, hornet->reward);
                        hornet->SwitchState("DashAttackAim");  // 在地面上, 发动冲刺攻击
                    } else {
                        hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy,
                                                            HornetAction::Jump, hornet->reward);
                        hornet->SwitchState("Jump");  // 没有在地面上尝试跳
                    }
                } else if (chance <= 60) {
                    hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy,
                                                        HornetAction::Walk, hornet->reward);
                    hornet->SwitchState("Walk");  // 走过去打
                } else if (chance <= 80) {
                    hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy,
                                                        HornetAction::Dash, hornet->reward);
                    hornet->SwitchState("Dash");  // dash 过去打
                } else if (chance <= 100) {
                    hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy,
                                                        HornetAction::Jump, hornet->reward);
                    hornet->SwitchState("Jump");  // 跳过去打
                }
                hornet->reward = 0.0f;
            }
        }
    }
}

// === evade ===
Evade::Evade(Hornet* hornet) : HornetStateNode(hornet) {}

void Evade::OnEnter()
{
    animator->SwitchToAnimation("evade");
    hornet->SetVelocityX(-hornet->dir * hornet->evadeSpeed);
    Audio::PlayAudio("hornetEvade");
    if (gameAf.Random(0, 100) <= 50) {
        int index = gameAf.Random(1, 2);
        Audio::PlayAudio(std::string{"hornetEvade"} + std::to_string(index));
    }
    hornet->currentAction = (int)HornetAction::Evade;
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
    HornetStateNode::OnEnter();
    animator->SwitchToAnimation("jump");
    jumpXSpeed = gameAf.Random(0.0f, hornet->maxJumpXSpeed);
    hornet->SetVelocity(
        Vector2{hornet->dir * jumpXSpeed,
                -1.0f * gameAf.Random(hornet->minJumpYSpeed, hornet->maxJumpYSpeed)});
    hornet->OnJumpVfx();
    Audio::PlayAudio("jump");
    int index = gameAf.Random(1, 3);
    Audio::PlayAudio(std::string{"hornetJump"} + std::to_string(index));
    hornet->currentAction = (int)HornetAction::Jump;
}

void Jump::OnUpdate()
{
    hornet->SetVelocityX(hornet->dir * jumpXSpeed);
    if (hornet->GetVelocity().Y >= 0) {
        hornet->SwitchState("Fall");
    }
    HornetStateNode::OnUpdate();
}

static inline void AirExecutiveDecision(Hornet* hornet)
{
    int chance = gameAf.Random(0, 100);
    if (hornet->PlayerInRange(hornet->evadeRange)) {
        // 如果在躲闪范围内
        if (chance <= 35) {
            hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy, HornetAction::AirDash,
                                                hornet->reward);
            hornet->reward = 0.0f;
            hornet->SwitchState("AirDash");
        } else if (hornet->isHostileState && chance <= 60) {
            // 普通攻击
            if (hornet->CanAttackHorizontal()) {
                hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy, HornetAction::Attack,
                                                    hornet->reward);
                hornet->SwitchState("Attack");
            } else {
                if (hornet->dstEnemy->GetPosition().Y < hornet->GetPosition().Y) {
                    // 在头上
                    hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy,
                                                        HornetAction::AttackUp, hornet->reward);
                    hornet->SwitchState("AttackUp");
                } else {
                    hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy,
                                                        HornetAction::AttackDown, hornet->reward);
                    hornet->SwitchState("AttackDown");
                }
            }
            hornet->reward = 0.0f;
        } else {
            hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy, HornetAction::DoNothing,
                                                hornet->reward);
            hornet->reward = -0.5f;
        }
    } else {
        // 如果不在躲闪范围内
        if (hornet->isHostileState) {
            if (chance <= 30) {
                hornet->OrientationPlayer();  // 朝向玩家方向冲刺
                hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy, HornetAction::AirDash,
                                                    hornet->reward);
                hornet->reward = 0.0f;
                hornet->SwitchState("AirDash");
            } else if (chance <= 60) {
                hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy,
                                                    HornetAction::DashAttackAim, hornet->reward);
                hornet->reward = 0.0f;
                hornet->SwitchState("DashAttackAim");  // 瞄准状态
            } else {
                // 40% 什么也不做
                hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy,
                                                    HornetAction::DoNothing, hornet->reward);
                hornet->reward = -0.5f;
            }
        }
    }
}

void Jump::ExecutiveDecision() { AirExecutiveDecision(hornet); }

// === Fall ===
Fall::Fall(Hornet* hornet) : HornetStateNode(hornet) {}

void Fall::OnEnter()
{
    HornetStateNode::OnEnter();
    animator->SwitchToAnimation("fall");
    hornet->currentAction = (int)HornetAction::Fall;
}

void Fall::OnUpdate()
{
    // 超出地图底下了
    if (hornet->GetPosition().Y >= 340.0f) {
        // 猜测只有hurt的时候被这样搞
        // 在追击时也会触发, 针对性测试下
        // 朝向中心(0, 0)
        // hornet->OrientationPlayer();  // 朝向玩家
        if (-hornet->GetPosition().X * hornet->dir < 0.0f) {
            // 朝向中心
            hornet->Flip();
        }
        hornet->SwitchState("RushJump");
    }

    if (hornet->isGround) {
        hornet->SwitchState("Idle");
        hornet->OnLandVfx();
        Audio::PlayAudio("land");
    }
    HornetStateNode::OnUpdate();
}

// 和jump一样的逻辑
void Fall::ExecutiveDecision() { AirExecutiveDecision(hornet); }

// === dash ===
Dash::Dash(Hornet* hornet) : HornetStateNode(hornet) {}

void Dash::OnEnter()
{
    // gameaf::log("dash dir: {}", hornet->dir);
    animator->SwitchToAnimation("dash");
    hornet->SetVelocityX(hornet->dir * hornet->dashSpeed);
    hornet->isInvincible = true;
    Audio::PlayAudio("hornetDash");
    hornet->currentAction = (int)HornetAction::Dash;
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
            hornet->SwitchState("Jump");  // 尝试跳跃过去?
        }
    }

    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        if (hornet->PlayerInRange(hornet->evadeRange) && gameAf.Random(0, 100) <= 50) {
            hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy, HornetAction::Attack,
                                                hornet->reward);
            hornet->SwitchState("Attack");
        } else {
            hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy, HornetAction::DoNothing,
                                                hornet->reward);
            if (hornet->isGround) {
                hornet->SwitchState("Idle");
            } else {
                hornet->SetVelocityX(hornet->dir * hornet->walkSpeed);
                hornet->SwitchState("Fall");
            }
        }
        hornet->reward = 0.0f;
    }
}

void Dash::OnExit() { hornet->isInvincible = false; }

// === air dash ===
AirDash::AirDash(Hornet* hornet) : HornetStateNode(hornet) {}

void AirDash::OnEnter()
{
    // gameaf::log("dash dir: {}", hornet->dir);
    animator->SwitchToAnimation("airDash");
    hornet->SetVelocityX(hornet->dir * hornet->dashSpeed);
    hornet->isInvincible = true;
    hornet->SetGravityEnabled(false);
    Audio::PlayAudio("hornetDash");
    hornet->currentAction = (int)HornetAction::AirDash;
}

void AirDash::OnUpdate()
{
    if (hornet->GetVelocity().X == 0.0f) {  // 碰到墙停下
        hornet->SwitchState("Fall");
    }

    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        if (hornet->PlayerInRange(hornet->evadeRange) && gameAf.Random(0, 100) <= 50) {
            if (hornet->CanAttackHorizontal()) {
                hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy, HornetAction::Attack,
                                                    hornet->reward);
                hornet->SwitchState("Attack");
            } else {
                if (hornet->dstEnemy->GetPosition().Y < hornet->GetPosition().Y) {
                    // 在头上
                    hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy,
                                                        HornetAction::AttackUp, hornet->reward);
                    hornet->SwitchState("AttackUp");
                } else {
                    hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy,
                                                        HornetAction::AttackDown, hornet->reward);
                    hornet->SwitchState("AttackDown");
                }
            }
        } else {
            hornet->SetVelocityX(hornet->dir * hornet->walkSpeed);
            hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy, HornetAction::DoNothing,
                                                hornet->reward);
            hornet->SwitchState("Fall");
        }
        hornet->reward = 0.0f;
    }
}

void AirDash::OnExit()
{
    hornet->isInvincible = false;
    hornet->SetGravityEnabled(true);
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
    Audio::PlayAudio("run", true);
    hornet->currentAction = (int)HornetAction::Walk;
}

void Walk::OnUpdate()
{
    walkDuration += gameAf.GetDeltaTime();
    // 如果即将坠落
    if (hornet->isAboutToFall || hornet->GetVelocity().X == 0.0f) {
        int chance = gameAf.Random(0, 100);
        if (chance <= 50) {
            hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy, HornetAction::Idle,
                                                hornet->reward);
            hornet->SwitchState("Idle");
        } else if (chance <= 65) {
            hornet->Flip();
            hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy, HornetAction::Jump,
                                                hornet->reward);
            hornet->SwitchState("Jump");
        } else {
            hornet->Flip();
            hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy, HornetAction::Dash,
                                                hornet->reward);
            hornet->SwitchState("Dash");
        }
        hornet->reward = 0.0f;
    }
    if (walkDuration >= currentWalkMaxDuration) {
        hornet->SwitchState("Idle");
    }
}

void Walk::OnExit() { Audio::StopAudio("run"); }

// === Hurt ===
Hurt::Hurt(Hornet* hornet) : HornetStateNode(hornet) {}

void Hurt::OnEnter()
{
    hornet->SetCollisonBoxOffsetY(100.0f);
    hornet->SetVelocity(hornet->currentAttackIntensity);
    animator->SwitchToAnimation("hurt");
    hornet->currentAction = (int)HornetAction::Idle;
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
    // gameaf::log("开始超级跳");
    animator->SwitchToAnimation("rushJump");
    jumpXSpeed = 0.0f;
    hornet->currentAction = (int)HornetAction::Jump;
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

// === Attack ===
Attack::Attack(Hornet* hornet) : HornetStateNode(hornet) {}

void Attack::OnEnter()
{
    if (gameAf.Random(0, 100) <= 50) {
        animator->SwitchToAnimation("attack1");
    } else {
        animator->SwitchToAnimation("attack2");
    }
    hornet->SetVelocity({});
    hornet->SetGravityEnabled(false);
    Audio::PlayAudio("hornetAttack");
    hornet->currentAction = (int)HornetAction::Attack;
}

void Attack::OnUpdate()
{
    if (animator->GetCurrentAnimation().GetCurrentFrameIndex() == 1) {
        hornet->AttackStart(Vector2{hornet->dir * 300.0f, -100.0f}, hornet->attackDamge);
    }
    if (animator->GetCurrentAnimation().GetCurrentFrameIndex() == 3) {
        hornet->AttackEnd();
    }
    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        if (gameAf.Random(0, 100) <= 50 && hornet->CanAttackHorizontal()) {
            hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy, HornetAction::Attack,
                                                hornet->reward);
            hornet->SwitchState("Attack");
        } else {
            hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy, HornetAction::Idle,
                                                hornet->reward);
            hornet->SwitchState("Idle");
        }
        hornet->reward = 0.0f;
    }
}

void Attack::OnExit() { hornet->SetGravityEnabled(true); }

// === AttackUp ===
AttackUp::AttackUp(Hornet* hornet) : HornetStateNode(hornet) {}

void AttackUp::OnEnter()
{
    hornet->SetVelocity({});
    hornet->SetGravityEnabled(false);
    hornet->AdjustAttackUpBox(true);
    animator->SwitchToAnimation("attackUp");
    Audio::PlayAudio("hornetAttack");
    hornet->currentAction = (int)HornetAction::AttackUp;
}

void AttackUp::OnUpdate()
{
    if (animator->GetCurrentAnimation().GetCurrentFrameIndex() == 1) {
        hornet->AttackStart(Vector2{0.0f, -500.0f}, hornet->attackUpDamge);
    }
    if (animator->GetCurrentAnimation().GetCurrentFrameIndex() == 2) {
        hornet->AttackEnd();
    }
    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        hornet->SwitchState("Fall");
    }
}

void AttackUp::OnExit()
{
    hornet->SetGravityEnabled(true);
    hornet->AdjustAttackUpBox(false);
}

// === AttackDown ===
AttackDown::AttackDown(Hornet* hornet) : HornetStateNode(hornet) {}

void AttackDown::OnEnter()
{
    hornet->SetVelocity({});
    hornet->SetGravityEnabled(false);
    hornet->AdjustAttackDownBox(true);
    animator->SwitchToAnimation("attackDown");
    Audio::PlayAudio("hornetAttack");
    hornet->currentAction = (int)HornetAction::AttackDown;
}

void AttackDown::OnUpdate()
{
    if (animator->GetCurrentAnimation().GetCurrentFrameIndex() == 2) {
        hornet->AttackStart(Vector2{hornet->dir * 300.0f, 300.0f}, hornet->attackDownDamge);
    }
    if (animator->GetCurrentAnimation().GetCurrentFrameIndex() == 3) {
        hornet->AttackEnd();
    }
    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        if (gameAf.Random(0, 100) <= 50) {
            hornet->SwitchState("Fall");
        } else {
            hornet->SwitchState("AttackBounce");
        }
    }
}

void AttackDown::OnExit()
{
    hornet->SetGravityEnabled(true);
    hornet->AdjustAttackDownBox(false);
}

// === AttackBounce ===
AttackBounce::AttackBounce(Hornet* hornet) : HornetStateNode(hornet) {}

void AttackBounce::OnEnter()
{
    animator->SwitchToAnimation("attackBounce");
    attackBounceXSpeed = gameAf.Random(0.0f, hornet->maxAttackBounceXSpeed);
    hornet->SetVelocity(Vector2{
        hornet->dir * attackBounceXSpeed,
        -1.0f * gameAf.Random(hornet->minAttackBounceYSpeed, hornet->maxAttackBounceYSpeed)});
}

void AttackBounce::OnUpdate()
{
    hornet->SetVelocityX(hornet->dir * attackBounceXSpeed);
    if (hornet->GetVelocity().Y >= 0) {
        hornet->SwitchState("Fall");
    }
}

// === Aim ===
DashAttackAim::DashAttackAim(Hornet* hornet) : HornetStateNode(hornet) {}

/// 负责瞄准玩家, 根据玩家现有的坐标
void DashAttackAim::OnEnter()
{
    if (hornet->isGround) {
        animator->SwitchToAnimation("dashAttackAimFloor");  // 地面瞄准
    } else {
        animator->SwitchToAnimation("dashAttackAimAir");  // 地面瞄准
    }

    hornet->SetVelocity({});
    hornet->SetGravityEnabled(false);
    hornet->currentAction = (int)HornetAction::DashAttackAim;
}

void DashAttackAim::OnUpdate()
{
    hornet->OrientationPlayer();  // 保持持续朝向玩家
    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        // 瞄准最后一帧播放结束后开始dash
        // 开始冲刺时, 决定冲刺方向和冲刺持续时间
        Vector2 playerPos = hornet->dstEnemy->GetPosition();
        Vector2 hornetPos = hornet->GetPosition();
        float dashAttacXDuration =
            (hornet->dashAttackPathScale * std::abs(playerPos.X - hornetPos.X)) /
            hornet->dashSpeed;  // 预留点距离
        if (hornet->isGround) {
            hornet->dashAttackDir = {hornet->dir, 0.0f};
            hornet->dashAttackDuration = dashAttacXDuration;
            hornet->SwitchState("DashAttackFloor");
        } else {
            // 玩家处于大黄蜂的上方
            if (playerPos.Y <= hornetPos.Y) {
                hornet->SwitchState("AirDash");
            } else {
                float dashAttacYDuration =
                    (hornet->dashAttackPathScale * std::abs(playerPos.Y - hornetPos.Y)) /
                    hornet->dashSpeed;
                hornet->dashAttackDuration = dashAttacYDuration > dashAttacXDuration
                                                 ? dashAttacYDuration
                                                 : dashAttacXDuration;
                hornet->dashAttackDir = (playerPos - hornetPos).Normalized();
                hornet->SwitchState("DashAttackAir");
            }
        }
        hornet->reward = 0.0f;
    }
}

void DashAttackAim::OnExit() { hornet->SetGravityEnabled(true); }

// === DashAttackFloor ===
DashAttackFloor::DashAttackFloor(Hornet* hornet) : HornetStateNode(hornet) {}

void DashAttackFloor::OnEnter()
{
    animator->SwitchToAnimation("dashAttackFloor");
    hornet->SetAttackBoxOffsetX(100);
    hornet->AttackStart({hornet->dir * 800.0f, 0.0f}, hornet->dashAttackDamge);
    hornet->OnDashAttackVfxFloor();
    Audio::PlayAudio("hornetRemoteSkill2");
}

void DashAttackFloor::OnUpdate()
{
    dashAttackDuration += gameAf.GetDeltaTime();
    hornet->SetVelocity(hornet->dashAttackDir * hornet->dashSpeed);
    if (dashAttackDuration >= hornet->dashAttackDuration) {
        hornet->SwitchState("Idle");
    }
}

void DashAttackFloor::OnExit()
{
    dashAttackDuration = 0.0f;
    hornet->SetAttackBoxOffsetX(-100);
    hornet->AttackEnd();
}

// === DashAttackAir ===
DashAttackAir::DashAttackAir(Hornet* hornet) : HornetStateNode(hornet) {}

void DashAttackAir::OnEnter()
{
    animator->SwitchToAnimation("dashAttackAir");
    hornet->AdjustAttackDownBox(true);
    hornet->AttackStart({hornet->dir * 800.0f, 0.0f}, hornet->dashAttackDamge);
    hornet->OnDashAttackVfxAir();
    Audio::PlayAudio("hornetRemoteSkill1");
}

void DashAttackAir::OnUpdate()
{
    dashAttackDuration += gameAf.GetDeltaTime();
    hornet->SetVelocity(hornet->dashAttackDir * hornet->dashSpeed);
    if (dashAttackDuration >= hornet->dashAttackDuration) {
        hornet->SwitchState("Idle");
    }
}

void DashAttackAir::OnExit()
{
    dashAttackDuration = 0.0f;
    hornet->AdjustAttackDownBox(false);
    hornet->AttackEnd();
}

// === Defend ===
Defend::Defend(Hornet* hornet) : HornetStateNode(hornet) {}

void Defend::OnEnter()
{
    hornet->isBlocking = true;
    animator->SwitchToAnimation("defend");
    hornet->currentAction = (int)HornetAction::Defend;
}

void Defend::OnUpdate()
{
    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        hornet->SwitchState("Idle");
    }
}

void Defend::OnExit() { hornet->isBlocking = false; }

// === DefendAttack ===
DefendAttack::DefendAttack(Hornet* hornet) : HornetStateNode(hornet) {}

void DefendAttack::OnEnter()
{
    animator->SwitchToAnimation("defendAttack");
    int index = gameAf.Random(1, 3);
    Audio::PlayAudio(std::string{"block"} + std::to_string(index));
}

void DefendAttack::OnUpdate()
{
    if (animator->GetCurrentAnimation().GetCurrentFrameIndex() == 3) {
        hornet->AttackStart(Vector2{hornet->dir * 800.0f, -400.0f}, hornet->defendAttackDamge);
    }
    if (animator->GetCurrentAnimation().GetCurrentFrameIndex() == 5) {
        hornet->AttackEnd();
    }
    if (animator->GetCurrentAnimation().IsEndOfPlay()) {
        hornet->SwitchState("Idle");
    }
}

// === Dead ===
Dead::Dead(Hornet* hornet) : HornetStateNode(hornet) {}

void Dead::OnEnter()
{
    animator->SwitchToAnimation("lowHealth");
    hornet->SetVelocity({});
    Audio::PlayAudio("hornetDead");
    hornet->trainingData.RecordDecision(hornet, hornet->dstEnemy, HornetAction::DoNothing,
                                        hornet->reward, true, -100.0f);
    hornet->reward = 0.0f;

    hornet->trainingData.Serialization();
    hornet->trainingData.Clear();
}

}  // namespace hornet