#pragma once

#include "../Character.hpp"
#include "../Common.hpp"

/**
 * 玩家控制对象
 */

using namespace gameaf;

class Player : public Character
{
public:
    Player() : Character(RenderZOrder::PLAYER, "player") {}

    void OnAwake() override;

    void OnUpdate() override;

    void OnDraw(const Camera& camera) override;

    GameObjectPtr Clone() override
    {
        auto clonePtr = std::make_shared<Player>(*this);
        if (auto parent = GetParent()) {
            parent->AddChildObject(clonePtr);
        }
        return clonePtr;
    }

public:
    /// @brief 设置x方向的速率
    void SetVelocityX(float x);

    /// @brief 设置y方向的速率
    void SetVelocityY(float y);

    /// @brief 设置速率
    void SetVelocity(const Vector2& v);

    /// @brief 获得当前的速率
    const Vector2& GetVelocity();

    /// @brief 玩家状态重新开始 恢复初始位置, 速度等
    void ReStart();

public:
    float xSpeed = 300.0f;     //水平方向的速度
    float jumpSpeed = 600.0f;  // 跳跃力

    bool isDebug = false;

private:
    Rigidbody2D* rb;
    CollisionBox* collisionBox;
    bool isRight = true;  // 默认情况下是朝右的
};