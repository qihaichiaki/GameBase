#include "Player.h"

#include <game_object/component/Animator.h>
#include <game_object/component/CollisionBox.h>
#include <game_object/component/Image.h>
#include <game_object/component/Rigidbody2D.h>
#include <game_object/component/Text.h>
#include <input/InputManager.h>

#include <common/Log.hpp>

#include "PlayerStateNodes.h"

void Player::OnAwake()
{
    // === 加载角色动画 ===
    auto animator = CreateComponent<Animator>();

    animator->AddAnimationForAtlas("idle", "player_idle", true, 0.05f);
    animator->AddAnimationForAtlas("idleToRun", "player_idleToRun", false, 0.1f);
    animator->AddAnimationForAtlas("run", "player_run", true, 0.1f);
    animator->AddAnimationForAtlas("runToIdle", "player_runToIdle", false, 0.1f);
    animator->AddAnimationForAtlas("jump", "player_jump", false, 0.1f);
    animator->AddAnimationForAtlas("falling", "player_falling", false, 0.1f);
    animator->AddAnimationForAtlas("landing", "player_landing", false, 0.1f);
    {
        animator->AddAnimationForAtlas("crouch", "player_crouch", true, 0.1f);
        // crouch 设置，循环帧在4~4
        animator->GetAnimation("crouch").SetLoopBeginFrameIndex(4);
    }
    animator->AddAnimationForAtlas("crouchingToIdle", "player_crouchingToIdle", false, 0.1f);

    animator->SetAnchorMode(ImageAnchorMode::BottomCentered);
    animator->SetSizeScale({2.5f, 2.5f});

    // 创建角色刚体
    rb = CreateComponent<Rigidbody2D>();
    collisionBox = CreateComponent<CollisionBox>(
        Vector2{0.0f, -animator->GetInitialAnimation().CurrentFrameSize().Y / 2});
    collisionBox->SetSrcLayer(CollisionLayerTool::player);
    collisionBox->AddDstLayer(CollisionLayerTool::wall);

    Flip();  // 缓存翻转状态(后续可优化)
    Flip();

    // 注册状态
    m_stateMachine.RegisterState("Idle", std::make_shared<Idle>(this));
    m_stateMachine.RegisterState("Run", std::make_shared<Run>(this));
    m_stateMachine.RegisterState("RunToIdle", std::make_shared<RunToIdle>(this));
    m_stateMachine.RegisterState("Jump", std::make_shared<Jump>(this));
    m_stateMachine.RegisterState("Falling", std::make_shared<Falling>(this));
    m_stateMachine.RegisterState("Landing", std::make_shared<Landing>(this));
    m_stateMachine.RegisterState("Crouch", std::make_shared<Crouch>(this));
    m_stateMachine.RegisterState("CrouchingToIdle", std::make_shared<CrouchingToIdle>(this));
    m_stateMachine.SetEntry("Idle");

    // 对过渡动画注入执行回调
    // 回调中转动画可以不用新建状态
    // 但是转状态, 需要在对应的状态操作, 否则会受到干扰
    animator->GetAnimation("idleToRun").SetOnFinished([this](Animation* animation) {
        GetComponent<Animator>()->SwitchToAnimation("run");
        animation->Restart();
    });

    animator->GetAnimation("runToIdle").SetOnFinished([this](Animation* animation) {
        m_stateMachine.SwitchTo("Idle");
        animation->Restart();
    });

    animator->GetAnimation("landing").SetOnFinished([this](Animation* animation) {
        m_stateMachine.SwitchTo("Idle");
        animation->Restart();
    });

    animator->GetAnimation("crouchingToIdle").SetOnFinished([this](Animation* animation) {
        m_stateMachine.SwitchTo("Idle");
        animation->Restart();
    });
}

void Player::OnUpdate() { Character::OnUpdate(); }

void Player::OnDraw(const Camera& camera)
{
    if (isDebug) collisionBox->OnDebugRender(camera);
}

void Player::SetVelocityX(float x) { SetVelocity({x, rb->Velocity().Y}); }

void Player::SetVelocityY(float y) { SetVelocity({rb->Velocity().X, y}); }

void Player::SetVelocity(const Vector2& v)
{
    rb->Velocity() = v;
    if (v.X < 0 && isRight || v.X > 0 && !isRight) {
        isRight = !isRight;
        Flip();  // 灵活
    }
}

const Vector2& Player::GetVelocity() { return rb->Velocity(); }

void Player::ReStart()
{
    SwitchState("Idle");
    SetPosition({});  // 玩家位置复原
    SetVelocity({});  // 速度恢复
}