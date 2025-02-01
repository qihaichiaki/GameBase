#include <game_af.h>
#include <game_object.h>
#include <resource_manager.h>
#include <scene.h>
#include <scene_manager.h>

using gameaf::GameObject;
using gameaf::ResourceManager;
using gameaf::Scene;
using gameaf::SceneManager;

class BackGround : public GameObject
{
public:
    BackGround(int z_redner = -1) : GameObject(z_redner) {}
    BackGround(const std::string& image_id, int z_redner = -1) : GameObject(z_redner)
    {
        GameAF::getInstance().log("加载img组件是否成功: {}", newImage(image_id));
        adaptImageSize();
    }
    BackGround(const BackGround&) = default;

    void onEnter() override {}
    void onUpdate() override {}
};

int main()
{
    auto& my_game = GameAF::getInstance();
    auto& reesource_manager = ResourceManager::getInstance();
    auto& scene_manager = SceneManager::getInstance();

    my_game.setShowConsole(true);
    my_game.initWindow();
    gameaf::log("{}", "游戏开始预加载设置...");
    gameaf::log("{}", "游戏开始加载图像资源...");
    reesource_manager.loadImage(
        R"(D:\project\GameBase\gamedemo\Assets\Graphics\Surroundings\Medieval_Castle\Background\layer_1.png)",
        "background-1");
    reesource_manager.loadImage(
        R"(D:\project\GameBase\gamedemo\Assets\Graphics\Surroundings\Medieval_Castle\Background\layer_2.png)",
        "background-2");

    gameaf::log("游戏开始加载场景资源......");
    scene_manager.registerState("main-scene", std::make_shared<Scene>());
    scene_manager.setEntry("main-scene");
    gameaf::log("游戏开始加载场景中游戏对象资源......");
    auto main_scene = scene_manager.getScene("main-scene");
    auto background = std::make_shared<BackGround>("background-1");
    background->scaleProportionally(4.0f);

    // 背景2由一个大的游戏对象, 三个小的游戏对象组成
    auto background2 = std::make_shared<BackGround>(0);
    auto background2_1 = std::make_shared<BackGround>("background-2");
    background2->addChildObject(background2_1);

    // background2
    // 设置背景2锚点为底部中间
    background2_1->setAnchorMode(GameObject::AnchorMode::BottomCentered);

    // 主场景添加游戏对象
    main_scene->addGameObjects({background2, background});
    // 设置游戏对象在主摄像机的中央
    main_scene->setCenterAnchorPoint(0, background);
    main_scene->setCenterAnchorPoint(0, background2_1);

    background2_1->scale({3.4f, 3.4f});
    background2_1->translate({0.0f, background->getSize().Y / 2.0f});
    // clone两份出来, 继承2_1的全部属性
    auto background2_2 = background2_1->clone();
    auto background2_3 = background2_1->clone();
    background2_2->translate({-background2_2->getSize().X, 0});
    background2_3->translate({background2_2->getSize().X, 0});

    gameaf::log("{}", "游戏开始运行...");
    my_game.run();
}