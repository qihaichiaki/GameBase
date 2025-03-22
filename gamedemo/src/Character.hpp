#pragma once

#include <game_object/GameObject.h>
#include <game_object/component/CollisionBox.h>
#include <game_object/component/Rigidbody2D.h>
#include <game_object/component/Text.h>

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

    void OnAwake() override
    {
        rb = CreateComponent<Rigidbody2D>();
        debugText = CreateComponent<Text>(std::wstring{L"Syne Mono"});
        debugText->SetAlignMode(TextAlignMode::CenterTop);
        debugText->SetEnabled(false);
    }

    void OnUpdate() override
    {
        stateMachine.OnUpdate();
        if (isDebug) {
            Vector2 pos = GetPosition();
            static std::wstringstream wss;
            wss.str(L"");
            wss << L"(" << pos.X << ", " << pos.Y << ")";
            debugText->SetText(wss.str());
        }
    }

    void OnDraw(const Camera& camera) override
    {
        if (isDebug) {
            collisionBox->OnDebugRender(camera);
        }
    }

public:
    /// @brief 切换状态
    /// @param stateId
    void SwitchState(const std::string& stateId) { stateMachine.SwitchTo(stateId); }

    /// @brief 设置x方向的速率
    void SetVelocityX(float x) { SetVelocity({x, rb->Velocity().Y}); }

    /// @brief 设置y方向的速率
    void SetVelocityY(float y) { SetVelocity({rb->Velocity().X, y}); }

    /// @brief 设置速率
    void SetVelocity(const Vector2& v)
    {
        rb->Velocity() = v;
        if (v.X < 0 && dir == 1.0f || v.X > 0 && dir == -1.0f) {
            dir = -1.0f * dir;
            Flip();  // 灵活
        }
    }

    /// @brief 获得当前的速率
    const Vector2& GetVelocity() { return rb->Velocity(); }

    virtual void SetDebug(bool isDebug)
    {
        this->isDebug = isDebug;
        if (isDebug) {
            debugText->SetEnabled(true);
        } else {
            debugText->SetEnabled(false);
        }
    }

    bool IsDebug() const { return isDebug; }

public:
    float dir;        // 朝向
    float xSpeed;     //水平方向的速度
    float jumpSpeed;  // 跳跃力

protected:
    StateMachine stateMachine;
    Rigidbody2D* rb;
    CollisionBox* collisionBox;  // 受击碰撞箱
    bool isDebug = false;        // 是否开启开发者模式

private:
    // debug 测试
    Text* debugText;
};