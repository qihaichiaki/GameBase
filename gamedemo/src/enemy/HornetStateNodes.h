#pragma once
#include <common/StateNode.hpp>
#include <game_object/component/Timer.hpp>

#include "Hornet.h"

/**
 * 大黄蜂npc状态节点
 */

namespace hornet {
class HornetStateNode : public StateNode
{
public:
    HornetStateNode(Hornet* hornet);

    void OnEnter() override;
    void OnUpdate() override;

    /// @brief 每当状态到达决策时间后会调用一次执行决策
    virtual void ExecutiveDecision() {};

protected:
    // 保护属性
    Hornet* hornet;
    Animator* animator;

    float decisionMakingTime = 0.0f;  // 当前状态的决策时间(enter时会在大黄蜂决策区间取值)
    float currentDuration = 0.0f;     // 当前持续时间, 用于判断是否执行决策
};

class Sit : public HornetStateNode
{
public:
    Sit(Hornet* hornet) : HornetStateNode(hornet) {}

    void OnEnter() override;
    void OnUpdate() override;
};

// 闲置状态
class Idle : public HornetStateNode
{
public:
    Idle(Hornet* hornet);

    void OnEnter() override;
    void OnUpdate() override;

    void ExecutiveDecision() override;
};

// evade 躲闪状态
class Evade : public HornetStateNode
{
public:
    Evade(Hornet* hornet);

    void OnEnter() override;
    void OnUpdate() override;
};

// jump 跳跃状态
class Jump : public HornetStateNode
{
public:
    Jump(Hornet* hornet);

    void OnEnter() override;
    void OnUpdate() override;

    void ExecutiveDecision() override;

private:
    float jumpXSpeed;
};

// fall 下落状态
class Fall : public HornetStateNode
{
public:
    Fall(Hornet* hornet);

    void OnEnter() override;
    void OnUpdate() override;

    void ExecutiveDecision() override;
};

// dash 状态
class Dash : public HornetStateNode
{
public:
    Dash(Hornet* hornet);

    void OnEnter() override;
    void OnUpdate() override;
    void OnExit() override;
};

class AirDash : public HornetStateNode
{
public:
    AirDash(Hornet* hornet);

    void OnEnter() override;
    void OnUpdate() override;
    void OnExit() override;
};

class Walk : public HornetStateNode
{
public:
    Walk(Hornet* hornet);

    void OnEnter() override;
    void OnUpdate() override;

private:
    float walkDuration = 0.0f;
    float currentWalkMaxDuration = 0.0f;
};

class Hurt : public HornetStateNode
{
public:
    Hurt(Hornet* hornet);

    void OnEnter() override;
    void OnUpdate() override;
    void OnExit() override;

private:
    float walkDuration = 0.0f;
    float currentWalkMaxDuration = 0.0f;
};

class RushJump : public HornetStateNode
{
public:
    RushJump(Hornet* hornet);

    void OnEnter() override;
    void OnUpdate() override;

private:
    float jumpXSpeed;
};

class Attack : public HornetStateNode
{
public:
    Attack(Hornet* hornet);

    void OnEnter() override;
    void OnUpdate() override;
    void OnExit() override;
};

class AttackUp : public HornetStateNode
{
public:
    AttackUp(Hornet* hornet);

    void OnEnter() override;
    void OnUpdate() override;
    void OnExit() override;
};

class AttackDown : public HornetStateNode
{
public:
    AttackDown(Hornet* hornet);

    void OnEnter() override;
    void OnUpdate() override;
    void OnExit() override;
};

class AttackBounce : public HornetStateNode
{
public:
    AttackBounce(Hornet* hornet);

    void OnEnter() override;
    void OnUpdate() override;

private:
    float attackBounceXSpeed;
};

class DashAttackAim : public HornetStateNode
{
public:
    DashAttackAim(Hornet* hornet);

    void OnEnter() override;
    void OnUpdate() override;
    void OnExit() override;
};

class DashAttackFloor : public HornetStateNode
{
public:
    DashAttackFloor(Hornet* hornet);

    void OnEnter() override;
    void OnUpdate() override;
    void OnExit() override;

private:
    float dashAttackDuration = 0.0f;
};

class DashAttackAir : public HornetStateNode
{
public:
    DashAttackAir(Hornet* hornet);

    void OnEnter() override;
    void OnUpdate() override;
    void OnExit() override;

private:
    float dashAttackDuration = 0.0f;
};

class Defend : public HornetStateNode
{
public:
    Defend(Hornet* hornet);

    void OnEnter() override;
    void OnUpdate() override;
    void OnExit() override;
};

class DefendAttack : public HornetStateNode
{
public:
    DefendAttack(Hornet* hornet);

    void OnEnter() override;
    void OnUpdate() override;
};

class Dead : public HornetStateNode
{
public:
    Dead(Hornet* hornet);

    void OnEnter() override;
};

}  // namespace hornet