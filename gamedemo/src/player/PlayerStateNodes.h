#pragma once

#include <common/StateNode.hpp>

#include "Player.h"

/**
 * 玩家状态节点
 * idle闲置状态
 */

class PlayerStateNode : public StateNode
{
public:
    PlayerStateNode(Player* player);

protected:
    Player* player;
    Animator* animator;
};

// 闲置状态
class Idle : public PlayerStateNode
{
public:
    Idle(Player* player) : PlayerStateNode(player) {}

    void OnEnter() override;
    void OnUpdate() override;
};

// 奔跑状态
class Run : public PlayerStateNode
{
public:
    Run(Player* player);

    void OnEnter() override;
    void OnUpdate() override;
};

// 奔跑转idle
class RunToIdle : public PlayerStateNode
{
public:
    RunToIdle(Player* player) : PlayerStateNode(player) {}

    void OnEnter() override;
    void OnUpdate() override;
};

// 跳跃状态
class Jump : public PlayerStateNode
{
public:
    Jump(Player* player) : PlayerStateNode(player) {}

    void OnEnter() override;
    void OnUpdate() override;
};

// 浮空状态
class Falling : public PlayerStateNode
{
public:
    Falling(Player* player) : PlayerStateNode(player) {}

    void OnEnter() override;
    void OnUpdate() override;
};

// 着落效果
class Landing : public PlayerStateNode
{
public:
    Landing(Player* player) : PlayerStateNode(player) {}

    void OnEnter() override;
    void OnUpdate() override;
};

// 蹲下状态
class Crouch : public PlayerStateNode
{
public:
    Crouch(Player* player) : PlayerStateNode(player) {}

    void OnEnter() override;
    void OnUpdate() override;
    void OnExit() override;

private:
    float offset = 30.0f;
};

// 过渡状态 蹲下->idle
class CrouchingToIdle : public PlayerStateNode
{
public:
    CrouchingToIdle(Player* player) : PlayerStateNode(player) {}

    void OnEnter() override;
    void OnUpdate() override;
};

// 翻滚状态 roll
class Roll : public PlayerStateNode
{
public:
    Roll(Player* player) : PlayerStateNode(player) {}

    void OnEnter() override;
    void OnUpdate() override;
};

// 翻滚状态->idle
class RollToIdle : public PlayerStateNode
{
public:
    RollToIdle(Player* player) : PlayerStateNode(player) {}

    void OnEnter() override;
    void OnUpdate() override;
};

// 攻击状态
class Attack : public PlayerStateNode
{
public:
    Attack(Player* player) : PlayerStateNode(player) {}
};

// 受击状态
class Hit : public PlayerStateNode
{
public:
    Hit(Player* player) : PlayerStateNode(player) {}
};

// 死亡状态
class Dead : public PlayerStateNode
{
public:
    Dead(Player* player) : PlayerStateNode(player) {}
};
