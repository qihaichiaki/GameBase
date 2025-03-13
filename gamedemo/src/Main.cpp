#include <GameAf.h>
#include <resource/ResourceManager.h>
#include <scene/SceneManager.h>

#include "./scene/MenuScene.hpp"

using namespace gameaf;

int main()
{
    auto& gameaf = GameAF::GetInstance();
    auto& sceneManager = SceneManager::GetInstance();
    auto& resourceManager = ResourceManager::GetInstance();

    // 初始化窗口
    // gameaf.SetShowConsole(true);
    gameaf.InitWindow(ASSETS_PATH "other/favicon.ico");

    // 游戏资源初始化
    resourceManager.LoadFont(ASSETS_PATH "");

    // 游戏场景加载
    sceneManager.RegisterState("menu", std::make_shared<MenuScene>());

    // 游戏死循环运行
    gameaf.Run();
    return 0;
}