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
    Run(Player* player) : PlayerStateNode(player) {}

    void OnEnter() override;
    void OnUpdate() override;
};

// 奔跑转idle
class RunToIdle : public PlayerStateNode
{
public:
    RunToIdle(Player* player) : PlayerStateNode(player) {}

    void OnEnter() override;
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
class Fall : public PlayerStateNode
{
public:
    Fall(Player* player) : PlayerStateNode(player) {}

    void OnEnter() override;
    void OnUpdate() override;
};

// 着落效果
class Land : public PlayerStateNode
{
public:
    Land(Player* player) : PlayerStateNode(player) {}

    void OnEnter() override;
};

// 冲刺状态 dash
class Dash : public PlayerStateNode
{
public:
    Dash(Player* player) : PlayerStateNode(player) {}
};

// 滑铲状态 slide
class Slide : public PlayerStateNode
{
public:
    Slide(Player* player) : PlayerStateNode(player) {}
};

// 悬挂状态 hang
class Hang : public PlayerStateNode
{
public:
    Hang(Player* player) : PlayerStateNode(player) {}
};

// 滑墙状态 WallSlide
class WallSlide : public PlayerStateNode
{
public:
    WallSlide(Player* player) : PlayerStateNode(player) {}
};

// 爬墙状态 WallClimb
class WallClimb : public PlayerStateNode
{
public:
    WallClimb(Player* player) : PlayerStateNode(player) {}
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
