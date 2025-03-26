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
    animator->AddAnimationForAtlas("airDash", "hornet_airDash", false, 0.05f);
    animator->AddAnimationForAtlas("evade", "hornet_evade", false, 0.05f);
    animator->AddAnimationForAtlas("hurt", "hornet_hurt", false, 0.1f);
    animator->AddAnimationForAtlas("attack1", "hornet_attack1", false, 0.1f);
    animator->AddAnimationForAtlas("attack2", "hornet_attack2", false, 0.1f);
    animator->AddAnimationForAtlas("attackUp", "hornet_attackUp", false, 0.1f);
    animator->AddAnimationForAtlas("attackDown", "hornet_attackDown", false, 0.1f);
    animator->AddAnimationForAtlas("attackBounce", "hornet_attackBounce", false, 0.1f);
    animator->AddAnimationForAtlas("dashAttackAimAir", "hornet_dashAttackAimAir", false, 0.1f);
    animator->AddAnimationForAtlas("dashAttackAimFloor", "hornet_dashAttackAimFloor", false, 0.1f);
    animator->AddAnimationForAtlas("dashAttackFloor", "hornet_dashAttackFloor", true, 0.1f);
    animator->AddAnimationForAtlas("dashAttackAir", "hornet_dashAttackAir", true, 0.1f);
    animator->AddAnimationForAtlas("idleToDefend", "hornet_idleToDefend", false, 0.1f);
    animator->AddAnimationForAtlas("defendToIdle", "hornet_defendToIdle", false, 0.1f);
    animator->AddAnimationForAtlas("defend", "hornet_defend", false, 0.1f);
    animator->AddAnimationForAtlas("defendAttack", "hornet_defendAttack", false, 0.1f);
    animator->AddAnimationForAtlas("lowHealth", "hornet_lowHealth", true, 0.1f);

    {
        // 对一些动画大小进行修正
        animator->GetAnimation("dashAttackAimAir").SetSizeScale({1.2f, 1.2f});
        animator->GetAnimation("dashAttackAimFloor").SetSizeScale({1.2f, 1.2f});
        animator->GetAnimation("dashAttackFloor").SetSizeScale({1.2f, 1.2f});
        animator->GetAnimation("dashAttackAir").SetSizeScale({1.2f, 1.2f});
    }

    // 加载过渡动画
    animator->AddTransitionAnimation("sit", "idle", "sitToIdle");
    animator->AddTransitionAnimation("idle", "walk", "idleToWalk");
    animator->AddTransitionAnimation("walk", "idle", "walkToIdle");
    animator->AddTransitionAnimation("idle", "defend", "idleToDefend");
    animator->AddTransitionAnimation("defend", "idle", "defendToIdle");

    // 受伤特效添加
    hurtVfx.AddFrame(ResourceManager::GetInstance().GetAtlas("hornet_vfxHurt1"));
    hurtVfx.SetSizeScale({0.5f, 0.5f});
    hurtVfx2.AddFrame(ResourceManager::GetInstance().GetAtlas("hornet_vfxHurt2"));
    hurtVfx2.SetSizeScale({0.5f, 0.5f});
    hurtVfx2.SetInterval(0.1f);
    hurtVfx2.SetOnFinished([this]() { isHurtVfxRender = false; });

    // 攻击特效添加
    attackVfx.AddFrame(ResourceManager::GetInstance().GetAtlas("hornet_vfxAttack1"));
    attackVfx2.AddFrame(ResourceManager::GetInstance().GetAtlas("hornet_vfxAttack2"));
    attackVfx.SetSizeScale({0.25f, 0.25f});
    attackVfx2.SetSizeScale({0.25f, 0.25f});
    attackVfx.SetInterval(0.1f);
    attackVfx2.SetInterval(0.1f);
    attackVfx.SetOnFinished([this]() { isAttackVfxRender = false; });

    // 初始化自身的攻击碰撞触发器
    attackBox->AddDstLayer(CollisionLayerTool::player);  // 对敌人造成伤害
    attackBox->SetSize(Vector2{250.0f, 100.0f});
    attackBox->SetOffset(Vector2{60.0f, 0.0f});

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
    stateMachine.RegisterState("AirDash", std::make_shared<hornet::AirDash>(this));
    stateMachine.RegisterState("Walk", std::make_shared<hornet::Walk>(this));
    stateMachine.RegisterState("Hurt", std::make_shared<hornet::Hurt>(this));
    stateMachine.RegisterState("RushJump", std::make_shared<hornet::RushJump>(this));
    stateMachine.RegisterState("Attack", std::make_shared<hornet::Attack>(this));
    stateMachine.RegisterState("AttackUp", std::make_shared<hornet::AttackUp>(this));
    stateMachine.RegisterState("AttackDown", std::make_shared<hornet::AttackDown>(this));
    stateMachine.RegisterState("AttackBounce", std::make_shared<hornet::AttackBounce>(this));
    stateMachine.RegisterState("DashAttackAim", std::make_shared<hornet::DashAttackAim>(this));
    stateMachine.RegisterState("DashAttackFloor", std::make_shared<hornet::DashAttackFloor>(this));
    stateMachine.RegisterState("DashAttackAir", std::make_shared<hornet::DashAttackAir>(this));
    stateMachine.RegisterState("Defend", std::make_shared<hornet::Defend>(this));
    stateMachine.RegisterState("DefendAttack", std::make_shared<hornet::DefendAttack>(this));
    stateMachine.RegisterState("Dead", std::make_shared<hornet::Dead>(this));

    stateMachine.SetEntry("Sit");

    // 设置初始位置
    SetPosition({670.0f, 250.0f});
}

void Hornet::OnUpdate()
{
    Character::OnUpdate();
    if (isHurtVfxRender) hurtVfx2.OnUpdate(gameAf.GetDeltaTime());
    if (isAttackVfxRender) {
        attackVfx.OnUpdate(gameAf.GetDeltaTime());
        attackVfx2.OnUpdate(gameAf.GetDeltaTime());
    }
}

void Hornet::OnDraw(const Camera& camera)
{
    Character::OnDraw(camera);
    if (isHurtVfxRender) hurtVfx2.OnRender(camera);
    if (isAttackVfxRender) {
        attackVfx.OnRender(camera);
        attackVfx2.OnRender(camera);
    }
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
    isHostileState = false;
    hp = maxHp;
    searchPlayerRange = 400.0f;
}

void Hornet::OnHurt(const Vector2& attackIntensity, int damage)
{
    if (isInvincible) {
        gameaf::log("大黄蜂是无敌状态!");
        return;
    }

    if (isBlocking && attackIntensity.X * dir < 0.0f) {
        isHitVfxRender = true;
        if (hitDir * attackIntensity.X < 0.0f) {
            hitDir *= -1.0f;
            hitVfx.Flip();
        }
        hitVfx.Restart();
        hitVfx.SetOffset(GetPosition());
        SwitchState("DefendAttack");  // 触发弹反
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

        if (hp > 0) {
            hp -= damage;
            gameaf::log("大黄蜂受到攻击 {}, 当前血量 {}", damage, hp);
            if (hp <= 0) {
                SwitchState("Dead");
                return;
            }

            currentAttackIntensity = attackIntensity;
            currentDamage = damage;
            if (!isHostileState) {
                isHostileState = true;
                searchPlayerRange = searchPlayerRangeHostile;
                if (attackIntensity.X * dir > 0.0f) {
                    Flip();  // 翻转一下
                }
                // 立即切换到hurt状态 -> 假设只在第一次不存在霸体, 后续存在霸体值(韧性条?)
                SwitchState("Hurt");
            }
        }
    }
}

void Hornet::OnAttack(Character* dstObj)
{
    gameaf::log("大黄蜂发动攻击.......");
    // if (attackDir * currentAttackIntensity.X < 0.0f) {
    //     attackDir *= -1.0f;
    //     attackVfx.Flip();
    //     attackVfx2.Flip();
    // }
    // isAttackVfxRender = true;
    // attackVfx.Restart();
    // attackVfx2.Restart();
    // attackVfx.SetOffset({GetPosition().X + attackDir * 100.0f, GetPosition().Y});
    // attackVfx2.SetOffset({GetPosition().X + attackDir * 100.0f, GetPosition().Y});
    Character::OnAttack(dstObj);
}

void Hornet::SetCollisonBoxOffsetY(float offsetY)
{
    collisionBox->SetSize({collisionBox->GetSize().X, collisionBox->GetSize().Y - offsetY});
}

void Hornet::SetAttackBoxOffsetX(float offsetX)
{
    attackBox->SetSize({attackBox->GetSize().X - offsetX, attackBox->GetSize().Y});
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

bool Hornet::CanAttackHorizontal() const
{
    Vector2 pos = GetPosition();
    Vector2 playerPos = player->GetPosition();
    return playerPos.Y <= pos.Y + collisionBox->GetSize().Y / 2 &&
           playerPos.Y >= pos.Y - collisionBox->GetSize().Y / 2 &&
           (playerPos - pos).X * dir >= 0.0f;
}

void Hornet::AdjustAttackUpBox(bool flag)
{
    Vector2 offset = attackBox->GetOffset();
    if (flag) {
        attackBox->SetSize(Vector2{100.0f, 250.0f});
        attackBox->SetOffset(Vector2{offset.X + dir * -60.0f, offset.Y});

    } else {
        attackBox->SetSize(Vector2{250.0f, 100.0f});
        attackBox->SetOffset(Vector2{offset.X + dir * 60.0f, offset.Y});
    }
}

void Hornet::AdjustAttackDownBox(bool flag)
{
    Vector2 offset = attackBox->GetOffset();
    if (flag) {
        attackBox->SetSize(Vector2{100.0f, 200.0f});
        attackBox->SetOffset(Vector2{offset.X + dir * -10.0f, offset.Y + 50.0f});

    } else {
        attackBox->SetSize(Vector2{250.0f, 100.0f});
        attackBox->SetOffset(Vector2{offset.X + dir * 10.0f, offset.Y - 50.0f});
    }
}