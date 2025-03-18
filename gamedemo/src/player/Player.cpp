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

    animator->AddAnimationForAtlas("idle", "player_idle", true, 0.063f);
    animator->AddAnimationForAtlas("idleToRun", "player_idleToRun", false, 0.1f);
    animator->AddAnimationForAtlas("run", "player_run", true, 0.1f);
    animator->AddAnimationForAtlas("runToIdle", "player_runToIdle", false, 0.1f);
    animator->AddAnimationForAtlas("jump", "player_jump", false, 0.1f);
    animator->AddAnimationForAtlas("falling", "player_falling", false, 0.1f);
    animator->AddAnimationForAtlas("landing", "player_landing", false, 0.1f);

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
    m_stateMachine.RegisterState("idle", std::make_shared<Idle>(this));
    m_stateMachine.RegisterState("run", std::make_shared<Run>(this));
    m_stateMachine.RegisterState("runToIdle", std::make_shared<RunToIdle>(this));
    m_stateMachine.RegisterState("jump", std::make_shared<Jump>(this));
    m_stateMachine.RegisterState("fall", std::make_shared<Fall>(this));
    m_stateMachine.RegisterState("land", std::make_shared<Land>(this));
    m_stateMachine.SetEntry("idle");

    // 对过渡动画注入执行回调
    // 回调中转动画可以不用新建状态
    // 但是转状态, 需要在对应的状态操作, 否则会受到干扰
    animator->GetAnimation("idleToRun").SetOnFinished([this](Animation* animation) {
        GetComponent<Animator>()->SwitchToAnimation("run");
        animation->Restart();
    });

    animator->GetAnimation("runToIdle").SetOnFinished([this](Animation* animation) {
        m_stateMachine.SwitchTo("idle");
        animation->Restart();
    });

    animator->GetAnimation("landing").SetOnFinished([this](Animation* animation) {
        m_stateMachine.SwitchTo("idle");
        animation->Restart();
    });
}

void Player::OnUpdate() { Character::OnUpdate(); }

void Player::OnDraw(const Camera& camera) { collisionBox->OnDebugRender(camera); }

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