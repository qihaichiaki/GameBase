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

protected:
    // 保护属性
    Hornet* hornet;
    Animator* animator;
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
    void OnExit() override;

private:
    float playerInEvadeDuration = 0.0f;           // 玩家进入躲闪范围持续时间
    float curentPlayerInEvadeMaxDuration = 0.0f;  // 当前玩家进入躲闪范围最大持续时间
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
};

// dash 状态
class Dash : public HornetStateNode
{
public:
    Dash(Hornet* hornet);

    void OnEnter() override;
    void OnUpdate() override;
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

}  // namespace hornet