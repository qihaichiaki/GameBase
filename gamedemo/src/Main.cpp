#include <GameAf.h>
#include <resource/ResourceManager.h>
#include <scene/SceneManager.h>

#include <chrono>
#include <thread>

#include "./scene/GameScene.hpp"
#include "./scene/LoadScene.hpp"
#include "./scene/MenuScene.hpp"

using namespace gameaf;

int main()
{
    auto& gameaf = GameAF::GetInstance();
    auto& sceneManager = SceneManager::GetInstance();
    auto& resourceManager = ResourceManager::GetInstance();

    // 初始化窗口
    gameaf.SetShowConsole(true);
    gameaf.InitWindow(ASSETS_PATH "favicon.ico");

    // 加载全局游戏资源
    resourceManager.LoadFont(ASSETS_PATH "SyneMono-Regular.ttf");

    // 游戏场景加载
    sceneManager.Register("menu", std::make_shared<MenuScene>());
    sceneManager.Register("game", std::make_shared<GameScene>());
    sceneManager.Register("load", std::make_shared<LoadScene>());
    // sceneManager.SetEntry("menu", true);
    sceneManager.LoadSceneAsync(
        [&]() {
            sceneManager.SetLoadProgress(0.0f);
            gameaf::log("执行任务1");
            std::this_thread::sleep_for(std::chrono::seconds(3));
            sceneManager.SetLoadProgress(0.2f);
            gameaf::log("执行任务2");
            std::this_thread::sleep_for(std::chrono::seconds(4));
            sceneManager.SetLoadProgress(0.6f);
            gameaf::log("执行任务3");
            std::this_thread::sleep_for(std::chrono::seconds(2));
            sceneManager.SetLoadProgress(1.0f);
            std::this_thread::sleep_for(std::chrono::seconds(2));
            gameaf::log("执行完毕");
        },
        "load", "menu", true);

    // 游戏死循环运行
    gameaf.Run();
    return 0;
}