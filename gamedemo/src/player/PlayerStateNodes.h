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

public:
    // 提供公共修改玩家竖直方向上box的方法(设计位移等)
    void SetSizeY(float offsetY);

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

// 攻击状态
class Attack : public PlayerStateNode
{
public:
    Attack(Player* player) : PlayerStateNode(player) {}

    void OnEnter() override;
};

// 地面攻击状态
class AttackStanding : public Attack
{
public:
    AttackStanding(Player* player) : Attack(player) {}

    void OnEnter() override;
    void OnUpdate() override;
};

// 空中攻击状态
class AttackAerial : public Attack
{
public:
    AttackAerial(Player* player) : Attack(player) {}

    void OnEnter() override;
    void OnUpdate() override;
};

// 蹲下攻击状态
class AttackCrouching : public Attack
{
public:
    AttackCrouching(Player* player) : Attack(player) {}

    void OnEnter() override;
    void OnUpdate() override;
};

class Blocking : public PlayerStateNode
{
public:
    Blocking(Player* player) : PlayerStateNode(player) {}

    void OnEnter() override;
    void OnUpdate() override;
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

}  // namespace player