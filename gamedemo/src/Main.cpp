#include <GameAf.h>
#include <scene/SceneManager.h>

#include <chrono>
#include <game_object/component/CollisionManager.hpp>

#include "./scene/AboutScene.hpp"
#include "./scene/GameScene.hpp"
#include "./scene/LoadScene.hpp"
#include "./scene/MenuScene.hpp"
#include "./scene/OptionsScene.hpp"
#include "LoadResource.hpp"

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
    sceneManager.Register("options", std::make_shared<OptionsScene>());
    sceneManager.Register("about", std::make_shared<AboutScene>());

    // 异步资源加载
    sceneManager.LoadSceneAsync(LoadResource, "load", "menu", true);

    // 游戏死循环运行
    gameaf.Run();
    return 0;
}