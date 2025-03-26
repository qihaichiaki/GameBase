#pragma once

#include <common/StateNode.hpp>

#include "Player.h"

/**
 * 玩家状态节点
 * idle闲置状态
 */
namespace player {

class PlayerStateNode : public StateNode
{
public:
    PlayerStateNode(Player* player);

protected:
    // 保护属性
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

// 翻滚状态 roll
class Roll : public PlayerStateNode
{
public:
    Roll(Player* player) : PlayerStateNode(player) {}

    void OnEnter() override;
    void OnUpdate() override;
    void OnExit() override;

private:
    float offset = 30.0f;
};

// 地面攻击状态
class AttackStanding : public PlayerStateNode
{
public:
    AttackStanding(Player* player) : PlayerStateNode(player) {}

    void OnEnter() override;
    void OnUpdate() override;
    void OnExit() override;
};

// 空中攻击状态
class AttackAerial : public PlayerStateNode
{
public:
    AttackAerial(Player* player) : PlayerStateNode(player) {}

    void OnEnter() override;
    void OnUpdate() override;
    void OnExit() override;
};

// 蹲下攻击状态
class AttackCrouching : public PlayerStateNode
{
public:
    AttackCrouching(Player* player) : PlayerStateNode(player) {}

    void OnEnter() override;
    void OnUpdate() override;
    void OnExit() override;
};

class Blocking : public PlayerStateNode
{
public:
    Blocking(Player* player) : PlayerStateNode(player) {}

    void OnEnter() override;
    void OnUpdate() override;
    void OnExit() override;

private:
    float hitDuration = 0.0f;  // 受击持续时间
};

// 受击状态
class Hurt : public PlayerStateNode
{
public:
    Hurt(Player* player) : PlayerStateNode(player) {}

    void OnEnter() override;
    void OnUpdate() override;
    void OnExit() override;

private:
    float hurtDuration = 0.0f;  // 受伤持续时间
};

// 死亡状态
class Dead : public PlayerStateNode
{
public:
    Dead(Player* player) : PlayerStateNode(player) {}

    void OnEnter() override;
};

}  // namespace player