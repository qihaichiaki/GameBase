#pragma once

#include <game_object/component/Animation.h>
#include <game_object/widgets/ProgressBar.h>

#include "../Character.h"
#include "../Common.hpp"
/**
 * 玩家控制对象
 */

class Player : public Character
{
public:
    Player(ProgressBar* hp, Camera* camera)
        : Character(RenderZOrder::PLAYER, "player"), hpProgressBar(hp), mainCamera(camera)
    {
    }

    void OnAwake() override;

    void OnUpdate() override;

    void OnDraw(const Camera& camera) override;

public:
    /// @brief 玩家状态重新开始 恢复初始位置, 速度等
    void ReStart();

    void SetVelocity(const Vector2& v) override;

    /// @brief 玩家受击
    void OnHurt(const Vector2& attackIntensity, int damage) override;

    /// @brief 玩家攻击
    /// @param dstObj
    void OnAttack(Character* dstObj) override;

    /// @brief 缩小角色受击碰撞箱的Y方向上的大小
    /// @param offsetY
    void SetCollisonBoxOffsetY(float offsetY);

    /// @brief 缩小角色攻击碰撞箱x方向上的大小
    /// @param offsetX
    void SetAttackBoxOffsetX(float offsetX);

    /// @brief 播放跳跃特效
    void OnJumpVfx();

    /// @brief 播放落地特效
    void OnLandVfx();

public:
    float xSpeed = 300.0f;     //水平方向的速度
    float jumpSpeed = 650.0f;  // 跳跃力
    float rollSpeed = 500.0f;  // 翻滚力

    int maxjumpNum = 1;                        // 空中允许的最大跳跃次数
    int jumpNum = maxjumpNum;                  // 空中当前跳跃次数
    int maxAttackAerialNum = 1;                // 空中允许的最大攻击次数
    int attackAerialNum = maxAttackAerialNum;  // 空中允许攻击次数

    // 跳跃宽限期
    Timer isOnFloorLastFrameTimer;
    bool isJumpForgiveExit = true;

    Vector2 hitAttackIntensity;         // 受击攻击强度
    bool isBlocking = false;            // 是否在格挡
    float blockHitMaxDuration = 0.75f;  // 抵御攻击后退持续时间

    float hurtMaxDuration = 0.15f;  // 受到伤害持续时间

    bool isHegemonicState = false;  // 是否霸体状态

    int attackStandingDamge = 6;
    int attackAirDamge = 5;

    float attackShakeIntensity = 0.0f;  // 当前攻击相机抖动强度
    float attackStandingShakeIntensity = 20.0f;
    float attackAerialShakeIntensity = 15.0f;
    float hurtShakeIntensity = 18.0f;

private:
    ProgressBar* hpProgressBar;
    Camera* mainCamera;
};