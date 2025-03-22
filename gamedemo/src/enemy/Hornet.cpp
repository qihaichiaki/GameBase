#include "Hornet.h"

#include <game_object/component/Animator.h>

#include "../player/Player.h"
#include "HornetStateNodes.h"

void Hornet::OnAwake()
{
    Character::OnAwake();
    dir = 1.0f;
    xSpeed = 350.0f;
    jumpSpeed = 700.0f;

    // === 加载大黄蜂动画 ===
    auto animator = CreateComponent<Animator>();
    animator->AddAnimationForImage("sit", "hornet_sit", false);
    animator->AddAnimationForImage("sitToIdle", "hornet_sitToIdle", false);
    animator->AddAnimationForAtlas("idle", "hornet_idle", true);
    animator->AddAnimationForAtlas("idleToWalk", "hornet_walkStart", false);
    animator->AddAnimationForAtlas("walk", "hornet_walk", true);
    animator->AddAnimationForAtlas("walkToIdle", "hornet_walkEnd", false);
    animator->AddAnimationForAtlas("jump", "hornet_jump", false);
    animator->AddAnimationForAtlas("fall", "hornet_fall", false);
    animator->AddAnimationForAtlas("rushJump", "hornet_rushJump", false);
    animator->AddAnimationForAtlas("dash", "hornet_dash", false);

    // 加载过渡动画
    animator->AddTransitionAnimation("sit", "idle", "sitToIdle");
    animator->AddTransitionAnimation("idle", "walk", "idleToWalk");
    animator->AddTransitionAnimation("walk", "idle", "walkToIdle");

    // 创建碰撞盒子
    collisionBox = CreateComponent<CollisionBox>();
    collisionBox->SetSrcLayer(CollisionLayerTool::enemy);
    collisionBox->AddDstLayer(CollisionLayerTool::wall);

    // 创建状态
    stateMachine.RegisterState("Sit", std::make_shared<hornet::Sit>(this));
    stateMachine.RegisterState("Idle", std::make_shared<hornet::Idle>(this));
    stateMachine.SetEntry("Sit");

    // 设置初始位置
    SetPosition({670.0f, 250.0f});
}

void Hornet::OnDraw(const Camera& camera) { Character::OnDraw(camera); }

void Hornet::OnUpdate()
{
    Character::OnUpdate();
    if (GetPosition().X - player->GetPosition().X > 0) {
        if (dir > 0) {
            dir = -1.0;
            Flip();
        }
    } else {
        if (dir < 0) {
            dir = 1.0;
            Flip();
        }
    }
}