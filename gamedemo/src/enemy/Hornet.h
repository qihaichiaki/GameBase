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

    /// @brief 播放跳跃特效
    void OnJumpVfx();

    /// @brief 播放落地特效
    void OnLandVfx();

public:
    bool isAboutToFall = false;  // 是否将要坠落

    float searchPlayerRange = 400.0f;  // 索敌范围
    float evadeRange = 180.0f;         // 躲闪范围

    float playerInEvadeMinDuration = 0.45f;  // 触发躲闪动作最小时间
    float playerInEvadeMaxDuration = 0.68f;  // 触发躲闪动作最大时间
    float evadeSpeed = 400.0f;               // 躲闪速度
    float dashSpeed = 800.0f;                // 冲刺速度

    // 普通跳
    float maxJumpXSpeed = 500.0f;
    float maxJumpYSpeed = 800.0f;
    float minJumpYSpeed = 450.0f;

    // 超级跳
    float minRushJumpXSpeed = 400.0f;
    float maxRushJumpXSpeed = 600.0f;
    float maxRushJumpYSpeed = 1250.0f;
    float minRushJumpYSpeed = 900.0f;

    float walkMinDuration = 0.92f;  // walk 最小持续时间
    float walkMaxDuration = 1.58f;  // walk 最大持续时间
    float walkSpeed = 350.0f;

    Vector2 hurtAttackIntensity;  // 受到攻击时的 hurt力量

    bool isBlocking = false;  // 是否在格挡

private:
    CollisionRaycaster* ray = nullptr;  // 射线检测, 检查前方是否存在悬崖

    Animation hurtVfx2;
};