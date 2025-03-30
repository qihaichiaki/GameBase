#include "Player.h"

#include <GameAf.h>
#include <game_object/component/Animator.h>
#include <game_object/component/CollisionBox.h>
#include <game_object/component/Image.h>
#include <game_object/component/Text.h>
#include <input/InputManager.h>
#include <resource/ResourceManager.h>

#include <common/Log.hpp>

#include "PlayerStateNodes.h"

static GameAf& gameAf = GameAf::GetInstance();

void Player::OnAwake()
{
    Character::OnAwake();

    dir = 1.0;  // 玩家默认朝右边

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
    animator->AddAnimationForImage("hitWhileBlocking", "player_hitWhileBlocking", false,
                                   0.1f);  // 盾格挡成功
    animator->AddAnimationForImage("hurt", "player_damageTaken", false,
                                   0.1f);  // 受到伤害
    animator->AddAnimationForAtlas("dead", "player_dead", false, 0.1f);

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

    // 受伤特效添加
    hurtVfx.AddFrame(ResourceManager::GetInstance().GetAtlas("player_vfxHurt"));
    hurtVfx.SetSizeScale({1.5f, 1.5f});  // 放大一下
    hurtVfx.SetOnFinished([this]() { isHurtVfxRender = false; });

    // 创建角色自身碰撞体
    collisionBox = CreateComponent<CollisionBox>(
        Vector2{0.0f, -animator->GetInitialAnimation().CurrentFrameSize().Y / 2});
    collisionBox->SetSrcLayer(CollisionLayerTool::player);
    collisionBox->AddDstLayer(CollisionLayerTool::wall);
    collisionBox->SetSize({40.0f, collisionBox->GetSize().Y});

    // 创建一个子对象用来角色检测地面碰撞体(创建子对象是因为不会影响父对象的刚体)

    groundDetectionCollision->SetSize(Vector2{30.0f, 5.0f});
    groundDetectionCollision->SetOffset(Vector2{0.0f, -1.5f});

    // 初始化自身的攻击碰撞触发器
    attackBox->AddDstLayer(dst);  // 对敌人造成伤害
    attackBox->SetSize(Vector2{230.0f, 100.0f});
    attackBox->SetOffset(
        Vector2{10.0f, -animator->GetInitialAnimation().CurrentFrameSize().Y / 2 - 50.0f});

    Flip();  // 缓存翻转状态(后续可优化)
    Flip();

    // 注册状态
    stateMachine.RegisterState("Idle", std::make_shared<player::Idle>(this));
    stateMachine.RegisterState("Run", std::make_shared<player::Run>(this));
    stateMachine.RegisterState("Jump", std::make_shared<player::Jump>(this));
    stateMachine.RegisterState("Falling", std::make_shared<player::Falling>(this));
    stateMachine.RegisterState("Crouch", std::make_shared<player::Crouch>(this));
    stateMachine.RegisterState("Roll", std::make_shared<player::Roll>(this));
    stateMachine.RegisterState("AttackStanding", std::make_shared<player::AttackStanding>(this));
    stateMachine.RegisterState("AttackAerial", std::make_shared<player::AttackAerial>(this));
    stateMachine.RegisterState("AttackCrouching", std::make_shared<player::AttackCrouching>(this));
    stateMachine.RegisterState("Blocking", std::make_shared<player::Blocking>(this));
    stateMachine.RegisterState("Hurt", std::make_shared<player::Hurt>(this));
    stateMachine.RegisterState("Dead", std::make_shared<player::Dead>(this));

    stateMachine.SetEntry("Idle");

    // 角色状态相关辅助更新
    isOnFloorLastFrameTimer.SetWaitTime(0.1f);
    isOnFloorLastFrameTimer.SetOnTimeout([&]() { isJumpForgiveExit = false; });
}

void Player::OnUpdate() { Character::OnUpdate(); }

void Player::OnDraw(const Camera& camera) { Character::OnDraw(camera); }

void Player::ReStart()
{
    SwitchState("Idle");
    SetPosition({});  // 玩家位置复原
    SetVelocity({});  // 速度恢复
    hp = maxHp;
    hpProgressBar->SetTargetProgressValue(1.0);
    hpProgressBar->GetComponent<Image>()->Load(
        ResourceManager::GetInstance().GetTImage("playerState1"));
}

void Player::SetVelocity(const Vector2& v)
{
    Character::SetVelocity(v);
    if (v.X < 0 && dir == 1.0f || v.X > 0 && dir == -1.0f) {
        dir = -1.0f * dir;
        Flip();  // 灵活
    }
}

void Player::OnHurt(const Vector2& attackIntensity, int damage)
{
    if (isInvincible) {
        gameaf::log("我是无敌的!");
        return;
    }

    Vector2 pos = GetPosition();
    hitAttackIntensity = attackIntensity;
    mainCamera->Shake(gameAf.Random(0.05f, 0.15f), hurtShakeIntensity);
    if (isBlocking && attackIntensity.X * dir < 0.0f) {
        isHitVfxRender = true;
        if (hitDir * attackIntensity.X < 0.0f) {
            hitDir *= -1.0f;
            hitVfx.Flip();
        }
        hitVfx.Restart();
        hitVfx.SetOffset({pos.X, pos.Y - collisionBox->GetSize().Y});
    } else {
        isHurtVfxRender = true;
        if (hurtDir * attackIntensity.X < 0.0f) {
            hurtDir *= -1.0f;
            hurtVfx.Flip();
        }
        hurtVfx.Restart();
        hurtVfx.SetOffset({pos.X, pos.Y - collisionBox->GetSize().Y});

        if (hp > 0) {
            hp -= damage;
            hpProgressBar->SetTargetProgressValue(hp * 1.0f / maxHp);
            gameaf::log("玩家受到伤害{}, 玩家剩余血量{}", damage, hp);

            if (hp <= 0.5 * maxHp && hp > 0.2 * maxHp) {
                hpProgressBar->GetComponent<Image>()->Load(
                    ResourceManager::GetInstance().GetTImage("playerState2"));
            } else if (hp <= 0.2 * maxHp) {
                hpProgressBar->GetComponent<Image>()->Load(
                    ResourceManager::GetInstance().GetTImage("playerState3"));
            }

            if (hp <= 0) {
                hpProgressBar->GetComponent<Image>()->Load(
                    ResourceManager::GetInstance().GetTImage("playerState4"));
                SwitchState("Dead");
                return;
            }
            if (!isHegemonicState) SwitchState("Hurt");
        }
    }
}

void Player::OnAttack(Character* dstObj)
{
    gameaf::log("玩家击打到了对方");
    Character::OnAttack(dstObj);
    if (!dstObj->isInvincible) mainCamera->Shake(gameAf.Random(0.05f, 0.15f), attackShakeIntensity);
}

void Player::SetCollisonBoxOffsetY(float offsetY)
{
    collisionBox->SetSize({collisionBox->GetSize().X, collisionBox->GetSize().Y - offsetY});
    collisionBox->SetOffset(
        {collisionBox->GetOffset().X, collisionBox->GetOffset().Y + offsetY / 2});
}

void Player::SetAttackBoxOffsetX(float offsetX)
{
    attackBox->SetSize({attackBox->GetSize().X - offsetX, attackBox->GetSize().Y});
}

void Player::OnJumpVfx()
{
    isjumpVfxRender = true;
    jumpVfx.Restart();
    jumpVfx.SetOffset(GetPosition());
}

void Player::OnLandVfx()
{
    islandVfxRender = true;
    landVfx.Restart();
    landVfx.SetOffset(GetPosition());
}