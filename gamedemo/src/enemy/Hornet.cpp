#include "Hornet.h"

#include <GameAf.h>
#include <game_object/component/Animator.h>
#include <game_object/component/CollisionBox.h>
#include <resource/ResourceManager.h>

#include "../player/Player.h"
#include "HornetStateNodes.h"

static GameAf& gameAf = GameAf::GetInstance();

void Hornet::OnAwake()
{
    Character::OnAwake();
    dir = 1.0f;

    // === 加载大黄蜂动画 ===
    auto animator = CreateComponent<Animator>();
    animator->AddAnimationForImage("sit", "hornet_sit", false);
    animator->AddAnimationForImage("sitToIdle", "hornet_sitToIdle", false, 0.25f);
    animator->AddAnimationForAtlas("idle", "hornet_idle", true, 0.1f);
    animator->AddAnimationForAtlas("idleToWalk", "hornet_walkStart", false, 0.1f);
    animator->AddAnimationForAtlas("walk", "hornet_walk", true, 0.05f);
    animator->AddAnimationForAtlas("walkToIdle", "hornet_walkEnd", false, 0.1f);
    animator->AddAnimationForAtlas("jump", "hornet_jump", false, 0.1f);
    animator->AddAnimationForAtlas("fall", "hornet_fall", true, 0.1f);
    animator->GetAnimation("fall").SetSizeScale(Vector2{124.0f / 165.0f, 124.0f / 165.0f});
    animator->AddAnimationForAtlas("rushJump", "hornet_rushJump", false, 0.05f);
    animator->AddAnimationForAtlas("dash", "hornet_dash", false, 0.05f);
    animator->AddAnimationForAtlas("evade", "hornet_evade", false, 0.05f);
    animator->AddAnimationForAtlas("hurt", "hornet_hurt", false, 0.1f);

    // 加载过渡动画
    animator->AddTransitionAnimation("sit", "idle", "sitToIdle");
    animator->AddTransitionAnimation("idle", "walk", "idleToWalk");
    animator->AddTransitionAnimation("walk", "idle", "walkToIdle");

    // 受伤特效添加
    hurtVfx.AddFrame(ResourceManager::GetInstance().GetAtlas("hornet_vfxHurt1"));
    hurtVfx.SetSizeScale({0.5f, 0.5f});
    hurtVfx2.AddFrame(ResourceManager::GetInstance().GetAtlas("hornet_vfxHurt2"));

    // 创建碰撞盒子
    collisionBox = CreateComponent<CollisionBox>();
    collisionBox->SetSrcLayer(CollisionLayerTool::enemy);
    collisionBox->AddDstLayer(CollisionLayerTool::wall);
    collisionBox->SetSize(Vector2{60.0f, collisionBox->GetSize().Y});

    // 地面检测盒子
    groundDetectionCollision->SetOffset(Vector2{0.0f, collisionBox->GetSize().Y / 2});
    groundDetectionCollision->SetSize(Vector2{40.0f, 5.0f});

    ray = CreateComponent<CollisionRaycaster>();
    ray->SetSrcLayer(CollisionLayerTool::enemy);
    ray->AddDstLayer(CollisionLayerTool::wall);
    ray->SetOffset(Vector2{20.0f, collisionBox->GetSize().Y / 2});
    ray->SetDir(RaycasterDir::Down);
    ray->SetLength(5.0f);
    ray->SetOnTriggerEnter([this](Collision*) { isAboutToFall = false; });
    ray->SetOnTriggerExit([this](Collision*) { isAboutToFall = true; });

    // 创建状态
    stateMachine.RegisterState("Sit", std::make_shared<hornet::Sit>(this));
    stateMachine.RegisterState("Idle", std::make_shared<hornet::Idle>(this));
    stateMachine.RegisterState("Evade", std::make_shared<hornet::Evade>(this));
    stateMachine.RegisterState("Jump", std::make_shared<hornet::Jump>(this));
    stateMachine.RegisterState("Fall", std::make_shared<hornet::Fall>(this));
    stateMachine.RegisterState("Dash", std::make_shared<hornet::Dash>(this));
    stateMachine.RegisterState("Walk", std::make_shared<hornet::Walk>(this));
    stateMachine.RegisterState("Hurt", std::make_shared<hornet::Hurt>(this));
    stateMachine.RegisterState("RushJump", std::make_shared<hornet::RushJump>(this));

    stateMachine.SetEntry("Sit");

    // 设置初始位置
    SetPosition({670.0f, 250.0f});
}

void Hornet::OnUpdate()
{
    Character::OnUpdate();
    if (isHurtVfxRender) hurtVfx2.OnUpdate(gameAf.GetDeltaTime());
}

void Hornet::OnDraw(const Camera& camera)
{
    Character::OnDraw(camera);
    if (isHurtVfxRender) hurtVfx2.OnRender(camera);
    if (isDebug) {
        ray->OnDebugRender(camera);
    }
}

void Hornet::OrientationPlayer()
{
    if (GetPosition().X - player->GetPosition().X > 0) {
        if (dir > 0) {
            Flip();
        }
    } else {
        if (dir < 0) {
            Flip();
        }
    }
}

void Hornet::Flip()
{
    dir *= -1.0f;
    Character::Flip();
    // gameaf::log("翻转");
}

void Hornet::ReStart()
{
    SwitchState("Sit");
    SetVelocity({});  // 速度恢复
    // 设置初始位置
    SetPosition({670.0f, 250.0f});
}

void Hornet::OnHurt(const Vector2& attackIntensity, int damage)
{
    if (isBlocking) {
        isHitVfxRender = true;
        if (hitDir * attackIntensity.X < 0.0f) {
            hitDir *= -1.0f;
            hitVfx.Flip();
        }
        hitVfx.Restart();
        hitVfx.SetOffset(GetPosition());
        gameaf::log("测试");
    } else {
        isHurtVfxRender = true;
        // if (hurtDir * attackIntensity.X < 0.0f) {
        //     hurtDir *= -1.0f;
        //     hurtVfx.Flip();
        // }
        hurtVfx.Restart();
        hurtVfx.SetOffset(GetPosition());
        hurtVfx2.Restart();
        hurtVfx2.SetOffset(GetPosition());

        gameaf::log("大黄蜂受到攻击 {}", damage);
        if (attackIntensity.X * dir > 0.0f) {
            Flip();  // 翻转一下
        }
        hurtAttackIntensity = attackIntensity;
        SwitchState("Hurt");  // 立即切换到hurt状态
    }
}

void Hornet::OnAttack(Character* dstObj)
{
    gameaf::log("大黄蜂发动攻击.......");
    Character::OnAttack(dstObj);
}

void Hornet::SetCollisonBoxOffsetY(float offsetY)
{
    collisionBox->SetSize({collisionBox->GetSize().X, collisionBox->GetSize().Y - offsetY});
}

void Hornet::OnJumpVfx()
{
    isjumpVfxRender = true;
    jumpVfx.Restart();
    Vector2 pos = GetPosition();
    jumpVfx.SetOffset({pos.X, pos.Y + collisionBox->GetSize().Y / 2.0f});
}

void Hornet::OnLandVfx()
{
    islandVfxRender = true;
    landVfx.Restart();
    Vector2 pos = GetPosition();
    landVfx.SetOffset({pos.X, pos.Y + collisionBox->GetSize().Y / 2.0f});
}