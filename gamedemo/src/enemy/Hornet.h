#pragma once

#include <game_object/component/Animation.h>
#include <game_object/component/CollisionRaycaster.h>

#include "Enemy.h"

/**
 * Boss Horent 敌人ai
 */

class Hornet : public Enemy
{
public:
    Hornet(Player* player) : Enemy("Horent", player) {}

    void OnAwake() override;

    void OnDraw(const Camera& camera) override;

    GameObjectPtr Clone() override
    {
        auto clonePtr = std::make_shared<Hornet>(*this);
        if (auto parent = GetParent()) {
            parent->AddChildObject(clonePtr);
        }
        return clonePtr;
    }

    void OnUpdate() override;

public:
    /// @brief 朝向玩家
    void OrientationPlayer();

    /// @brief 自定义翻转对象
    void Flip();

    /// @brief 重置大黄蜂状态
    void ReStart();

    /// @brief 大黄蜂受击
    void OnHurt(const Vector2& attackIntensity, int damage) override;

    /// @brief 大黄蜂攻击
    /// @param dstObj
    void OnAttack(Character* dstObj) override;

    /// @brief 缩小大黄蜂y方向上的碰撞盒大小
    /// @param offsetY
    void SetCollisonBoxOffsetY(float offsetY);

    /// @brief 缩小大黄蜂攻击碰撞箱x方向上的大小
    /// @param offsetX
    void SetAttackBoxOffsetX(float offsetX);

    /// @brief 播放跳跃特效
    void OnJumpVfx();

    /// @brief 播放落地特效
    void OnLandVfx();

    /// @brief 是否可以水平攻击 即玩家在大黄蜂的正前方
    bool CanAttackHorizontal() const;

    /// @brief 调整向上攻击时的攻击碰撞箱
    void AdjustAttackUpBox(bool flag);

    /// @brief 调整向下攻击时的攻击碰撞箱
    void AdjustAttackDownBox(bool flag);

public:
    bool isAboutToFall = false;  // 是否将要坠落

    float searchPlayerRange = 400.0f;          // 当前/初始索敌范围
    float searchPlayerRangeHostile = 1000.0f;  // 敌意状态下的索敌范围
    float evadeRange = 220.0f;                 // 躲闪范围

    // 决策时间 - 在大黄蜂的每一个状态下, 会取下面决策区间作为一次当前决策时间
    // 对应每个决策时间会进行一次决策(就是针对玩家的下一次动作)
    float minDecisionMakingTime = 0.42f;  // 最小决策时间
    float maxDecisionMakingTime = 0.60f;  // 最大决策时间

    float evadeSpeed = 450.0f;  // 躲闪速度
    float dashSpeed = 800.0f;   // 冲刺速度

    // 普通跳
    float maxJumpXSpeed = 500.0f;
    float maxJumpYSpeed = 800.0f;
    float minJumpYSpeed = 450.0f;

    // 超级跳
    float minRushJumpXSpeed = 400.0f;
    float maxRushJumpXSpeed = 600.0f;
    float maxRushJumpYSpeed = 1250.0f;
    float minRushJumpYSpeed = 900.0f;

    float maxAttackBounceXSpeed = 200.0f;
    float minAttackBounceYSpeed = 250.0f;
    float maxAttackBounceYSpeed = 400.0f;

    float walkMinDuration = 0.92f;  // walk 最小持续时间
    float walkMaxDuration = 1.58f;  // walk 最大持续时间
    float walkSpeed = 350.0f;

    Vector2 dashAttackDir;                    // 冲刺方向
    float dashAttackDuration = 0.0f;          // 冲刺攻击的持续时间
    float dashAttackPathScale = 4.0f / 3.0f;  // 最终实际参与计算的路径长度占比

    bool isBlocking = false;      // 是否在格挡
    bool isHostileState = false;  // 对玩家的仇恨状态

    int attackDamge = 5;
    int attackUpDamge = 8;
    int attackDownDamge = 10;
    int dashAttackDamge = 4;
    int defendAttackDamge = 15;

private:
    CollisionRaycaster* ray = nullptr;  // 射线检测, 检查前方是否存在悬崖

    Animation hurtVfx2;
    Animation attackVfx2;
    bool isAttackVfxRender = false;
};