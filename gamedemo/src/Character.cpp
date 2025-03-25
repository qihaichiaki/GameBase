#include "Character.h"

#include <GameAf.h>
#include <game_object/component/CollisionBox.h>
#include <game_object/component/Rigidbody2D.h>
#include <game_object/component/Text.h>
#include <resource/ResourceManager.h>

static GameAf& gameAf = GameAf::GetInstance();

void Character::OnAwake()
{
    rb = CreateComponent<Rigidbody2D>();
    debugText = CreateComponent<Text>(std::wstring{L"Syne Mono"});
    debugText->SetAlignMode(TextAlignMode::CenterTop);
    debugText->SetEnabled(false);

    // 攻击碰撞箱
    attackBox = CreateComponent<CollisionBox>();
    attackBox->SetTrigger(true);                       // 作为触发器使用
    attackBox->SetSrcLayer(CollisionLayerTool::none);  // 自身不参与碰撞检测
    attackBox->SetEnabled(false);                      // 默认情况下不会进行启用
    attackBox->SetOnTriggerEnter([this](Collision* dst) {
        // 触发回调
        auto dstObj = static_cast<Character*>(dst->GetGameObject());
        gameaf::log("触发回调");
        if (!dstObj->isInvincible) OnAttack(dstObj);
    });

    // 地面检测碰撞器
    auto groundDetection = std::make_shared<GameObject>();
    AddChildObject(groundDetection);
    groundDetectionCollision = groundDetection->CreateComponent<CollisionBox>();
    groundDetectionCollision->SetTrigger(true);  // 作为触发器使用, 用来检测地面
    groundDetectionCollision->SetSrcLayer(CollisionLayerTool::none);
    groundDetectionCollision->AddDstLayer(CollisionLayerTool::wall);
    groundDetectionCollision->SetOnTriggerEnter([&](Collision* dst) { isGround = true; });
    groundDetectionCollision->SetOnTriggerExit([&](Collision* dst) { isGround = false; });

    {  // 特效动画管理 -> 自己管理
        // 跳跃和落地特效
        jumpVfx.AddFrame(Image{nullptr, ResourceManager::GetInstance().GetTImage("vfxJump")});
        landVfx.AddFrame(Image{nullptr, ResourceManager::GetInstance().GetTImage("vfxLand")});
        jumpVfx.SetOnFinished([this]() { isjumpVfxRender = false; });
        landVfx.SetOnFinished([this]() { islandVfxRender = false; });

        jumpVfx.SetInterval(0.05f);
        landVfx.SetInterval(0.1f);
        jumpVfx.SetAnchorMode(ImageAnchorMode::BottomCentered);
        landVfx.SetAnchorMode(ImageAnchorMode::BottomCentered);

        // 被打击和被伤害特效
        hitVfx.AddFrame(ResourceManager::GetInstance().GetAtlas("vfxHit"));
        // 被伤害特效帧各自添加(可能会不一样)
        hitVfx.SetOnFinished([this]() { isHitVfxRender = false; });
        hurtVfx.SetOnFinished([this]() { isHurtVfxRender = false; });

        hitVfx.SetInterval(0.1f);
        hurtVfx.SetInterval(0.1f);
        hitVfx.SetAnchorMode(ImageAnchorMode::Centered);
        hurtVfx.SetAnchorMode(ImageAnchorMode::Centered);
    }
}

void Character::OnUpdate()
{
    stateMachine.OnUpdate();
    if (isDebug) {
        Vector2 pos = GetPosition();
        static std::wstringstream wss;
        wss.str(L"");
        wss << L"(" << pos.X << ", " << pos.Y << ")";
        debugText->SetText(wss.str());
    }

    if (isjumpVfxRender) jumpVfx.OnUpdate(gameAf.GetDeltaTime());
    if (islandVfxRender) landVfx.OnUpdate(gameAf.GetDeltaTime());
    if (isHitVfxRender) hitVfx.OnUpdate(gameAf.GetDeltaTime());
    if (isHurtVfxRender) hurtVfx.OnUpdate(gameAf.GetDeltaTime());
}

void Character::OnDraw(const Camera& camera)
{
    if (isDebug) {
        collisionBox->OnDebugRender(camera);
        groundDetectionCollision->OnDebugRender(camera);
        attackBox->OnDebugRender(camera);
    }
    // 渲染特效
    if (isjumpVfxRender) jumpVfx.OnRender(camera);
    if (islandVfxRender) landVfx.OnRender(camera);
    if (isHitVfxRender) hitVfx.OnRender(camera);
    if (isHurtVfxRender) hurtVfx.OnRender(camera);
}

void Character::SetVelocityX(float x) { SetVelocity({x, rb->Velocity().Y}); }

void Character::SetVelocityY(float y) { SetVelocity({rb->Velocity().X, y}); }

void Character::SetVelocity(const Vector2& v) { rb->Velocity() = v; }

const Vector2& Character::GetVelocity() { return rb->Velocity(); }

void Character::SetDebug(bool isDebug)
{
    this->isDebug = isDebug;
    if (isDebug) {
        debugText->SetEnabled(true);
    } else {
        debugText->SetEnabled(false);
    }
}

void Character::AttackStart(const Vector2& attackIntensity, int damage)
{
    currentAttackIntensity = attackIntensity;
    currentDamage = damage;
    attackBox->SetEnabled(true);
}

void Character::AttackEnd() { attackBox->SetEnabled(false); }