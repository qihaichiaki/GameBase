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
            sceneManager.SetLoadProgress(1.0f / 3);
            std::this_thread::sleep_for(std::chrono::seconds(2));
            // ui_choose图集
            resourceManager.LoadAtlas(ASSETS_PATH "effect/ui_choose/%d.png", 11, "ui_choose");
            sceneManager.SetLoadProgress(2.0f / 3);
            std::this_thread::sleep_for(std::chrono::seconds(2));
            // 加载测试背景
            resourceManager.NewImage(ASSETS_PATH "background/test.jpg", "test");
            // 加载玩家精灵图
            resourceManager.NewImage(ASSETS_PATH "character/player/player.png", "player", 17, 6,
                                     99);
            // 加载游戏背景
            resourceManager.NewImage(ASSETS_PATH "background/layer_1.png", "background_layer1");
            resourceManager.NewImage(ASSETS_PATH "background/layer_2.png", "background_layer2");
            resourceManager.NewImage(ASSETS_PATH "platform/TearCity/Floor_1.png", "ground");
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