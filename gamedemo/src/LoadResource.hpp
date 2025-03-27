#pragma once

#include <resource/ResourceManager.h>
#include <scene/SceneManager.h>

#include <thread>

/**
 * 异步加载资源区域
 */

using namespace gameaf;

inline void LoadResource(SceneManager& sceneManager)
{
    auto& resourceManager = ResourceManager::GetInstance();
    // 加载实际资源
    sceneManager.SetLoadProgress(0.0f);
    // bug 飞蝶图集
    resourceManager.LoadAtlas(ASSETS_PATH "effect/bug/%d.png", 8, "bug");
    sceneManager.SetLoadProgress(1.0f / 4);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // ui_choose图集
    resourceManager.LoadAtlas(ASSETS_PATH "effect/ui_choose/%d.png", 11, "ui_choose");
    sceneManager.SetLoadProgress(2.0f / 4);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // 加载测试背景
    resourceManager.NewImage(ASSETS_PATH "background/test.jpg", "test");
    // 加载玩家图集
    resourceManager.LoadAtlas(ASSETS_PATH "character/player/idle/%d.png", 56, "player_idle");
    resourceManager.LoadAtlas(ASSETS_PATH "character/player/idleToRun/%d.png", 2,
                              "player_idleToRun");
    resourceManager.LoadAtlas(ASSETS_PATH "character/player/run/%d.png", 8, "player_run");
    resourceManager.LoadAtlas(ASSETS_PATH "character/player/runToIdle/%d.png", 4,
                              "player_runToIdle");
    resourceManager.LoadAtlas(ASSETS_PATH "character/player/jump/%d.png", 5, "player_jump");
    resourceManager.LoadAtlas(ASSETS_PATH "character/player/falling/%d.png", 3, "player_falling");
    resourceManager.LoadAtlas(ASSETS_PATH "character/player/landing/%d.png", 3, "player_landing");
    resourceManager.LoadAtlas(ASSETS_PATH "character/player/crouch/%d.png", 5, "player_crouch");
    resourceManager.LoadAtlas(ASSETS_PATH "character/player/crouchingToIdle/%d.png", 1,
                              "player_crouchingToIdle");
    resourceManager.LoadAtlas(ASSETS_PATH "character/player/roll/%d.png", 5, "player_roll");
    resourceManager.LoadAtlas(ASSETS_PATH "character/player/rollToIdle/%d.png", 4,
                              "player_rollToIdle");
    // attack_twoHanded_standing
    resourceManager.NewImage(ASSETS_PATH
                             "character/player/attack_twoHanded_standing/attack_standing.png",
                             "player_attack_standing", 1, 19);
    // attack_aerial
    resourceManager.NewImage(ASSETS_PATH
                             "character/player/attack_oneHanded_aerial/attack_aerial.png",
                             "player_attack_aerial", 1, 19);
    // attack_crouching
    resourceManager.NewImage(
        ASSETS_PATH "character/player/attack_missileLauncher_crouching/attack_crouching.png",
        "player_attack_crouching", 1, 12);
    // attack block
    resourceManager.NewImage(ASSETS_PATH "character/player/attackBlocked.png",
                             "player_attackBlocked");
    resourceManager.NewImage(ASSETS_PATH "character/player/attackBlocked_crouching.png",
                             "player_attackBlocked_crouching");
    // shield 盾
    resourceManager.NewImage(ASSETS_PATH "character/player/blockingWithShield.png",
                             "player_blockingWithShield");  // 盾格挡
    resourceManager.NewImage(ASSETS_PATH "character/player/hitWhileBlocking.png",
                             "player_hitWhileBlocking");  // 盾格挡成功
    // damageTaken
    resourceManager.NewImage(ASSETS_PATH "character/player/damageTaken.png",
                             "player_damageTaken");  // 受到伤害
    // dead
    resourceManager.LoadAtlas(ASSETS_PATH "character/player/dead/%d.png", 9, "player_dead");

    // jump vfx
    resourceManager.NewImage(ASSETS_PATH "effect/vfx_jump.png", "vfxJump", 1,
                             5);  // 跳起特效

    // land vfx
    resourceManager.NewImage(ASSETS_PATH "effect/vfx_land.png", "vfxLand", 1,
                             2);  // 跳起特效

    // hit vfx 受击特效
    resourceManager.LoadAtlas(ASSETS_PATH "effect/hit/%d.png", 4, "vfxHit");
    // hurt vfx 玩家受伤特效
    resourceManager.LoadAtlas(ASSETS_PATH "effect/hurt/%d.png", 18, "player_vfxHurt");

    sceneManager.SetLoadProgress(3.0f / 4);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 加载大黄蜂图集
    // idle
    resourceManager.NewImage(ASSETS_PATH "character/hornet/other/SitDown.png", "hornet_sit");
    resourceManager.NewImage(ASSETS_PATH "character/hornet/other/StandUp.png", "hornet_sitToIdle");
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/Idle/%d.png", 6, "hornet_idle");
    // walk
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/WalkStart/%d.png", 6,
                              "hornet_walkStart");
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/Walk/%d.png", 8, "hornet_walk");
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/WalkEnd/%d.png", 3,
                              "hornet_walkEnd");
    // jump
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/Jump/%d.png", 6, "hornet_jump");
    // fall
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/fall/%d.png", 4, "hornet_fall");
    // rushJump
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/RushJump/%d.png", 14,
                              "hornet_rushJump");
    // dash
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/Dash/%d.png", 9, "hornet_dash");
    // air dash
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/AirDash/%d.png", 8,
                              "hornet_airDash");
    // evade
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/Evade/%d.png", 7, "hornet_evade");
    // hurt
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/Hurt/%d.png", 6, "hornet_hurt");
    // attack 1
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/Attack_0/%d.png", 5,
                              "hornet_attack1");
    // attack 2
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/Attack_1/%d.png", 5,
                              "hornet_attack2");
    // attack up
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/AttackUp/%d.png", 5,
                              "hornet_attackUp");
    // attack down
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/AttackDown/%d.png", 5,
                              "hornet_attackDown");
    // AttackBounce
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/AttackBounce/%d.png", 11,
                              "hornet_attackBounce");
    // dash attack aim 需要放大1.2倍正常
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/aim/%d.png", 9,
                              "hornet_dashAttackAimAir", true);
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/squat/%d.png", 10,
                              "hornet_dashAttackAimFloor", true);
    // dash attack floor 需要放大1.2倍正常
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/dash_on_floor/%d.png", 2,
                              "hornet_dashAttackFloor", true);
    // dash attack air 需要放大1.2倍正常
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/dash_in_air/%d.png", 2,
                              "hornet_dashAttackAir", true);
    // defend 格挡
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/DefendStart/%d.png", 3,
                              "hornet_idleToDefend");
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/DefendEnd/%d.png", 2,
                              "hornet_defendToIdle");
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/Defend/%d.png", 4,
                              "hornet_defend");
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/DefendAttack/%d.png", 7,
                              "hornet_defendAttack");

    // lowHealth
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/LowHealth/%d.png", 6,
                              "hornet_lowHealth");

    // 大黄蜂相关特效
    // 大黄蜂受伤特效?
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/HurtEffect/%d.png", 4,
                              "hornet_vfxHurt1");
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/HurtEffect_/%d.png", 7,
                              "hornet_vfxHurt2");
    // 攻击特效
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/AttackEffect/%d.png", 4,
                              "hornet_vfxAttack1");
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/AttackEffect_/%d.png", 3,
                              "hornet_vfxAttack2");
    // dash attack 特效 需要放大1.2倍正常
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/vfx_dash_on_floor/%d.png", 6,
                              "hornet_vfxDashAttackFloor", true);
    resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/vfx_dash_in_air/%d.png", 5,
                              "hornet_vfxDashAttackAir", true);

    // 加载游戏背景
    resourceManager.NewImage(ASSETS_PATH "background/layer_1.png", "background_layer1");
    resourceManager.NewImage(ASSETS_PATH "background/layer_2.png", "background_layer2");
    resourceManager.NewImage(ASSETS_PATH "platform/TearCity/Floor_1.png", "ground");
    resourceManager.NewImage(ASSETS_PATH "platform/TearCity/ruin_plat_float_02.png", "wall");

    // 加载就警告框
    resourceManager.LoadAtlas(ASSETS_PATH "effect/warning/%d.png", 7, "warning");

    // 加载角色状态图
    resourceManager.NewImage(ASSETS_PATH "background/playerState1.png", "playerState1");
    resourceManager.NewImage(ASSETS_PATH "background/playerState2.png", "playerState2");
    resourceManager.NewImage(ASSETS_PATH "background/playerState3.png", "playerState3");
    resourceManager.NewImage(ASSETS_PATH "background/playerState4.png", "playerState4");

    sceneManager.SetLoadProgress(1.0f);
    // 等待2秒缓冲
    std::this_thread::sleep_for(std::chrono::seconds(2));
    gameaf::log("资源加载完毕");
}