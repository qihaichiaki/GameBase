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

    animator->AddAnimationForAtlas("idle", "player_idle", true, 0.045f);
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
    animator->AddAnimationForAtlas("roll", "player_roll", false, 0.1f);
    animator->AddAnimationForAtlas("rollToIdle", "player_rollToIdle", false, 0.1f);
    animator->AddAnimationForImage("attack_standing", "player_attack_standing", false,
                                   0.06f);  // 地面双手武器攻击
    animator->AddAnimationForImage("attack_aerial", "player_attack_aerial", false,
                                   0.048f);  // 空中单手武器攻击
    animator->AddAnimationForImage("attack_crouching", "player_attack_crouching", false,
                                   0.1f);  // 蹲下火箭攻击
    animator->AddAnimationForImage("blocking", "player_blockingWithShield", false, 0.1f);  // 盾格挡

    {
        // 设置过渡动画
        animator->AddTransitionAnimation("idle", "run", "idleToRun");
        animator->AddTransitionAnimation("run", "idle", "runToIdle");
        animator->AddTransitionAnimation("crouch", "idle", "crouchingToIdle");
        animator->AddTransitionAnimation("roll", "idle", "rollToIdle");
        animator->AddTransitionAnimation("falling", "idle", "landing");
    }

    animator->SetAnchorMode(ImageAnchorMode::BottomCentered);
    animator->SetSizeScale({2.5f, 2.5f});

    // 创建角色刚体
    rb = CreateComponent<Rigidbody2D>();
    // 创建角色自身碰撞体
    collisionBox = CreateComponent<CollisionBox>(
        Vector2{0.0f, -animator->GetInitialAnimation().CurrentFrameSize().Y / 2});
    collisionBox->SetSrcLayer(CollisionLayerTool::player);
    collisionBox->AddDstLayer(CollisionLayerTool::wall);
    collisionBox->SetSize({40.0f, collisionBox->GetSize().Y});

    // 创建一个子对象用来角色检测地面碰撞体(创建子对象是因为不会影响父对象的刚体)
    auto groundDetection = std::make_shared<GameObject>();
    AddChildObject(groundDetection);
    groundDetectionCollision = groundDetection->CreateComponent<CollisionBox>();
    groundDetectionCollision->SetSrcLayer(CollisionLayerTool::player);
    groundDetectionCollision->AddDstLayer(CollisionLayerTool::wall);
    groundDetectionCollision->SetSize(Vector2{40.0f, 5.0f});
    groundDetectionCollision->SetOffset(Vector2{0.0f, -1.5f});
    groundDetectionCollision->SetTrigger(true);  // 作为触发器使用, 用来检测地面
    groundDetectionCollision->SetOnTriggerEnter([&](Collision* dst) { isGround = true; });
    groundDetectionCollision->SetOnTriggerExit([&](Collision* dst) { isGround = false; });

    Flip();  // 缓存翻转状态(后续可优化)
    Flip();

    // 注册状态
    m_stateMachine.RegisterState("Idle", std::make_shared<Idle>(this));
    m_stateMachine.RegisterState("Run", std::make_shared<Run>(this));
    m_stateMachine.RegisterState("Jump", std::make_shared<Jump>(this));
    m_stateMachine.RegisterState("Falling", std::make_shared<Falling>(this));
    m_stateMachine.RegisterState("Crouch", std::make_shared<Crouch>(this));
    m_stateMachine.RegisterState("Roll", std::make_shared<Roll>(this));
    m_stateMachine.RegisterState("AttackStanding", std::make_shared<AttackStanding>(this));
    m_stateMachine.RegisterState("AttackAerial", std::make_shared<AttackAerial>(this));
    m_stateMachine.RegisterState("AttackCrouching", std::make_shared<AttackCrouching>(this));
    m_stateMachine.RegisterState("Blocking", std::make_shared<Blocking>(this));
    m_stateMachine.SetEntry("Idle");

    // 角色状态相关辅助更新
    isOnFloorLastFrameTimer.SetWaitTime(0.1f);
    isOnFloorLastFrameTimer.SetOnTimeout([&]() { isJumpForgiveExit = false; });
}

void Player::OnUpdate() { Character::OnUpdate(); }

void Player::OnDraw(const Camera& camera)
{
    if (isDebug) {
        collisionBox->OnDebugRender(camera);
        groundDetectionCollision->OnDebugRender(camera);
    }
}

void Player::SetVelocityX(float x) { SetVelocity({x, rb->Velocity().Y}); }

void Player::SetVelocityY(float y) { SetVelocity({rb->Velocity().X, y}); }

void Player::SetVelocity(const Vector2& v)
{
    rb->Velocity() = v;
    if (v.X < 0 && dir == 1.0f || v.X > 0 && dir == -1.0f) {
        dir = -1.0f * dir;
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