#pragma once

#include <game_object/component/Timer.hpp>

#include "../Character.hpp"
#include "../Common.hpp"
/**
 * 玩家控制对象
 */

class Player : public Character
{
public:
    Player() : Character(RenderZOrder::PLAYER, "player") {}

    void OnAwake() override;

    void OnUpdate() override;

    void OnDraw(const Camera& camera) override;

public:
    /// @brief 玩家状态重新开始 恢复初始位置, 速度等
    void ReStart();

public:
    float rollSpeed = 500.0f;  // 翻滚力

    int maxjumpNum = 1;                        // 空中允许的最大跳跃次数
    int jumpNum = maxjumpNum;                  // 空中当前跳跃次数
    int maxAttackAerialNum = 1;                // 空中允许的最大攻击次数
    int attackAerialNum = maxAttackAerialNum;  // 空中允许攻击次数

    bool isGround = false;  // 是否在地面上

    // 跳跃宽限期
    Timer isOnFloorLastFrameTimer;
    bool isJumpForgiveExit = true;

private:
    CollisionBox* groundDetectionCollision;
};