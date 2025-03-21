#include <GameAf.h>
#include <resource/ResourceManager.h>
#include <scene/SceneManager.h>

#include <chrono>
#include <game_object/component/CollisionManager.hpp>
#include <thread>

#include "./scene/GameScene.hpp"
#include "./scene/LoadScene.hpp"
#include "./scene/MenuScene.hpp"

using namespace gameaf;

int main()
{
    // === 初始化窗口 第一个进行初始化 ===
    GameAf::InitWindow(ASSETS_PATH "favicon.ico", true);

    auto& gameaf = GameAf::GetInstance();
    auto& sceneManager = SceneManager::GetInstance();
    auto& resourceManager = ResourceManager::GetInstance();

    // === 游戏场景加载 ===
    sceneManager.Register("load", std::make_shared<LoadScene>());
    sceneManager.Register("menu", std::make_shared<MenuScene>());
    sceneManager.Register("game", std::make_shared<GameScene>());

    // 异步资源加载
    sceneManager.LoadSceneAsync(
        [&]() {
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
            resourceManager.LoadAtlas(ASSETS_PATH "character/player/idle/%d.png", 56,
                                      "player_idle");
            resourceManager.LoadAtlas(ASSETS_PATH "character/player/idleToRun/%d.png", 2,
                                      "player_idleToRun");
            resourceManager.LoadAtlas(ASSETS_PATH "character/player/run/%d.png", 8, "player_run");
            resourceManager.LoadAtlas(ASSETS_PATH "character/player/runToIdle/%d.png", 4,
                                      "player_runToIdle");
            resourceManager.LoadAtlas(ASSETS_PATH "character/player/jump/%d.png", 5, "player_jump");
            resourceManager.LoadAtlas(ASSETS_PATH "character/player/falling/%d.png", 3,
                                      "player_falling");
            resourceManager.LoadAtlas(ASSETS_PATH "character/player/landing/%d.png", 3,
                                      "player_landing");
            resourceManager.LoadAtlas(ASSETS_PATH "character/player/crouch/%d.png", 5,
                                      "player_crouch");
            resourceManager.LoadAtlas(ASSETS_PATH "character/player/crouchingToIdle/%d.png", 1,
                                      "player_crouchingToIdle");
            resourceManager.LoadAtlas(ASSETS_PATH "character/player/roll/%d.png", 5, "player_roll");
            resourceManager.LoadAtlas(ASSETS_PATH "character/player/rollToIdle/%d.png", 4,
                                      "player_rollToIdle");
            // attack_twoHanded_standing
            resourceManager.NewImage(
                ASSETS_PATH "character/player/attack_twoHanded_standing/attack_standing.png",
                "player_attack_standing", 1, 19);
            // attack_aerial
            resourceManager.NewImage(ASSETS_PATH
                                     "character/player/attack_oneHanded_aerial/attack_aerial.png",
                                     "player_attack_aerial", 1, 19);
            // attack_crouching
            resourceManager.NewImage(
                ASSETS_PATH
                "character/player/attack_missileLauncher_crouching/attack_crouching.png",
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

            sceneManager.SetLoadProgress(3.0f / 4);
            std::this_thread::sleep_for(std::chrono::seconds(1));

            // 加载大黄蜂图集
            // idle
            resourceManager.NewImage(ASSETS_PATH "character/hornet/other/SitDown.png",
                                     "hornet_sit");
            resourceManager.NewImage(ASSETS_PATH "character/hornet/other/StandUp.png",
                                     "hornet_sitToIdle");
            resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/Idle/%d.png", 6,
                                      "hornet_idle");
            // walk
            resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/WalkStart/%d.png", 6,
                                      "hornet_walkStart");
            resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/Walk/%d.png", 8,
                                      "hornet_walk");
            resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/WalkEnd/%d.png", 3,
                                      "hornet_walkEnd");
            // jump
            resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/Jump/%d.png", 6,
                                      "hornet_jump");
            // fall
            resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/Fall/%d.png", 4,
                                      "hornet_fall");
            // rushJump
            resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/RushJump/%d.png", 14,
                                      "hornet_rushJump");
            // dash
            resourceManager.LoadAtlas(ASSETS_PATH "character/hornet/other/Dash/%d.png", 9,
                                      "hornet_dash");

            // 加载游戏背景
            resourceManager.NewImage(ASSETS_PATH "background/layer_1.png", "background_layer1");
            resourceManager.NewImage(ASSETS_PATH "background/layer_2.png", "background_layer2");
            resourceManager.NewImage(ASSETS_PATH "platform/TearCity/Floor_1.png", "ground");
            resourceManager.NewImage(ASSETS_PATH "platform/TearCity/ruin_plat_float_02.png",
                                     "wall");

            sceneManager.SetLoadProgress(1.0f);
            // 等待2秒缓冲
            std::this_thread::sleep_for(std::chrono::seconds(2));
            gameaf::log("资源加载完毕");
        },
        "load", "menu", true);

    // 游戏死循环运行
    gameaf.Run();
    return 0;
}