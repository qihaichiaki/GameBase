#pragma once

#include <game_object/GameObject.h>
#include <game_object/component/Animation.h>

#include <common/StateMachine.hpp>
#include <sstream>

/**
 * 角色基类, 所有角色都共有的属性:
 * 1. hp
 * 2. 状态机
 * ......
 */

using namespace gameaf;

class Character : public GameObject
{
public:
    Character(int zOrder, const std::string& name) : GameObject(zOrder, name) {}

    void OnAwake() override;

    void OnUpdate() override;

    void OnDraw(const Camera& camera) override;

public:
    /// @brief 切换状态
    /// @param stateId
    void SwitchState(const std::string& stateId) { stateMachine.SwitchTo(stateId); }

    /// @brief 设置x方向的速率
    void SetVelocityX(float x);

    /// @brief 设置y方向的速率
    void SetVelocityY(float y);

    /// @brief 设置速率
    virtual void SetVelocity(const Vector2& v);

    /// @brief 获得当前的速率
    const Vector2& GetVelocity();

    /// @brief 对象设置debug显示模式
    virtual void SetDebug(bool isDebug);

    bool IsDebug() const { return isDebug; }

    /// @brief 受到攻击的回调, 其中存在对方攻击的强度 + 伤害
    virtual void OnHurt(const Vector2& attackIntensity, int damage) {};

    /// @brief 开始攻击的回调
    virtual void OnAttack(Character* dstObj)
    {
        dstObj->OnHurt(currentAttackIntensity, currentDamage);
    }

    /// @brief 设置本次攻击强度和伤害量
    void AttackStart(const Vector2& attackIntensity, int damage);

    /// @brief 结束本次攻击(关闭攻击碰撞箱)
    void AttackEnd();

    /// @brief 设置是否重力模拟
    void SetGravityEnabled(bool isEnabled);

public:
    int hp = 0;     // 公共 hp
    int maxHp = 0;  // 最大血量

    Vector2 currentAttackIntensity;
    int currentDamage = 0;

    float dir;              // 朝向
    bool isGround = false;  // 是否在地面上

    bool isInvincible = false;  // 是否无敌

    int currentAction = 0;  // 当前动作id, 方便后续的强化学习训练

protected:
    StateMachine stateMachine;
    Rigidbody2D* rb;
    CollisionBox* collisionBox;              // 受击碰撞箱
    CollisionBox* attackBox;                 // 攻击碰撞箱
    CollisionBox* groundDetectionCollision;  // 地面碰撞箱
    bool isDebug = false;                    // 是否开启开发者模式

    // 公共特效特殊管理动画
    bool isjumpVfxRender = false;
    bool islandVfxRender = false;
    bool isHitVfxRender = false;
    bool isHurtVfxRender = false;
    Animation jumpVfx;     // 跳起特效 -> 公共使用, 会在此处初始化
    Animation landVfx;     // 落下特效 -> 公共使用, 会在此处初始化
    Animation attackVfx;   // 攻击特效?
    int hitDir = -1.0f;    // 决定受击特效方向
    int hurtDir = 1.0f;    // 决定受到伤害特效方向
    int attackDir = 1.0f;  // 攻击伤害特效方向
    Animation hitVfx;      // 被打击特效(格挡)
    Animation hurtVfx;     // 被伤害特效

private:
    // debug 测试
    Text* debugText;
};