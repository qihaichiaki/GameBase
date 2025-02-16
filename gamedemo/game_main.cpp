#include <camera.h>
#include <component_tool.h>
#include <game_af.h>
#include <game_object.h>
#include <input_manager.h>
#include <resource_manager.h>
#include <scene.h>
#include <scene_manager.h>

using gameaf::AnimatorTool;
using gameaf::Camera;
using gameaf::GameObject;
using gameaf::ImageTool;
using gameaf::InputManager;
using gameaf::ResourceManager;
using gameaf::Scene;
using gameaf::SceneManager;
using gameaf::Vector2;

enum ZOrderLevel { z_background_back, z_background_front, z_player };

class BackGround : public GameObject
{
public:
    BackGround(int z_redner = ZOrderLevel::z_background_back) : GameObject(z_redner) {}
    BackGround(const std::string& image_id, int z_redner = ZOrderLevel::z_background_back)
        : GameObject(z_redner)
    {
        ImageTool::create(*this, image_id);
    }
    BackGround(const BackGround&) = default;
};

class Player : public GameObject
{
public:
    Player()
    {
        setName("Player");
        setZOrder(ZOrderLevel::z_player);
        // newImage("player-idle1");
        // adaptImageSize();
        AnimatorTool::newAnimationForAtlas(*this, "idle-right", "player-idle-right", 0.15f, true);
        AnimatorTool::newAnimationForAtlas(*this, "idle-left", "player-idle-left", 0.15f, true);
        AnimatorTool::newAnimationForAtlas(*this, "run-right", "player-run-right", 0.15f, true);
        AnimatorTool::newAnimationForAtlas(*this, "run-left", "player-run-left", 0.15f, true);

        AnimatorTool::setInitialAnimation(*this, "run-right");
    }

    void onUpdate() override
    {
        using KeyValue = InputManager::KeyValue;
        auto& input_manager = InputManager::getInstance();
        auto& game_af = GameAF::getInstance();

        if (input_manager.isKeyDown(KeyValue::A)) {
            is_left = true;
        }
        if (input_manager.isKeyDown(KeyValue::D)) {
            is_right = true;
        }
        if (input_manager.isKeyDown(KeyValue::W)) {
            is_up = true;
        }
        if (input_manager.isKeyDown(KeyValue::S)) {
            is_down = true;
        }

        if (input_manager.isKeyUp(KeyValue::A)) {
            is_left = false;
        }
        if (input_manager.isKeyUp(KeyValue::D)) {
            is_right = false;
        }
        if (input_manager.isKeyUp(KeyValue::W)) {
            is_up = false;
        }
        if (input_manager.isKeyUp(KeyValue::S)) {
            is_down = false;
        }

        if (is_left && is_up)
            velocity = {-(float)std::sqrt(1.0 / 2), -(float)std::sqrt(1.0 / 2)};
        else if (is_left && is_down)
            velocity = {-(float)std::sqrt(1.0 / 2), (float)std::sqrt(1.0 / 2)};
        else if (is_right && is_up)
            velocity = {(float)std::sqrt(1.0 / 2), -(float)std::sqrt(1.0 / 2)};
        else if (is_right && is_down)
            velocity = {(float)std::sqrt(1.0 / 2), (float)std::sqrt(1.0 / 2)};
        else if (is_left)
            velocity = {-1.0f, 0.0f};
        else if (is_right)
            velocity = {1.0f, 0.0f};
        else if (is_up)
            velocity = {0.0f, -1.0f};
        else if (is_down)
            velocity = {0.0f, 1.0f};
        else
            velocity = {};

        // if (is_left || is_right || is_up || is_down) {
        //     gameaf::log("当前玩家位置{}", getPosition());
        // }

        auto d_x = velocity * speed * game_af.getDeltaTime();
        // if (d_x.X != 0.0f || d_x.Y != 0.0f) gameaf::log("移动距离为: {}", d_x);
        translate(d_x);
        // auto [s_x, s_y] = game_af.getScreenSize();
        // if (getPosition().X > s_x) setPosition({(float)s_x, getPosition().Y});
        // if (getPosition().X < 0) setPosition({0, getPosition().Y});
        // if (getPosition().Y > s_y) setPosition({getPosition().X, (float)s_y});
        // if (getPosition().Y < 0) setPosition({getPosition().X, 0});

        if (is_dir_left && is_right) {
            is_dir_left = false;
        } else if (!is_dir_left && is_left) {
            is_dir_left = true;
        }

        if (is_right) AnimatorTool::switchToAnimation(*this, "run-right");

        if (is_left) AnimatorTool::switchToAnimation(*this, "run-left");

        if (is_dir_left && !is_left) AnimatorTool::switchToAnimation(*this, "idle-left");
        if (!is_dir_left && !is_right) AnimatorTool::switchToAnimation(*this, "idle-right");
    }

private:
    Vector2 velocity;
    float speed = 1000.0f;
    bool is_left = false;
    bool is_right = false;
    bool is_up = false;
    bool is_down = false;
    bool is_dir_left = false;
};

int main()
{
    auto& my_game = GameAF::getInstance();
    auto& resource_manager = ResourceManager::getInstance();
    auto& scene_manager = SceneManager::getInstance();

    // my_game.setFPS(60);
    my_game.setShowConsole(true);
    my_game.initWindow();
    gameaf::log("{}", "游戏开始预加载设置...");
    gameaf::log("{}", "游戏开始加载图像资源...");
    resource_manager.loadImage(
        R"(D:\project\GameBase\gamedemo\Assets\Graphics\Surroundings\Medieval_Castle\Background\layer_1.png)",
        "background-1");
    resource_manager.loadImage(
        R"(D:\project\GameBase\gamedemo\Assets\Graphics\Surroundings\Medieval_Castle\Background\layer_2.png)",
        "background-2");

    // 加载idle图集
    resource_manager.loadAtlas(R"(D:\project\gamedemo\KongDongWuShi\resources\enemy\idle\%d.png)",
                               6, "player-idle-left");
    resource_manager.flipAtlas("player-idle-left", "player-idle-right");
    resource_manager.loadAtlas(R"(D:\project\gamedemo\KongDongWuShi\resources\enemy\run\%d.png)", 8,
                               "player-run-left");
    resource_manager.flipAtlas("player-run-left", "player-run-right");

    gameaf::log("游戏开始加载场景资源......");
    scene_manager.registerState("main-scene", std::make_shared<Scene>());
    scene_manager.setEntry("main-scene");
    gameaf::log("游戏开始加载场景中游戏对象资源......");
    auto main_scene = scene_manager.getScene("main-scene");
    auto background = std::make_shared<BackGround>("background-1");
    ImageTool::scaleProportionally(*background, 4.0f);
    background->setName("background-1");

    // 背景2由一个大的游戏对象, 三个小的游戏对象组成
    auto background2 = std::make_shared<BackGround>(ZOrderLevel::z_background_front);
    auto background2_1 = std::make_shared<BackGround>("background-2");
    background2->addChildObject(background2_1);

    // background2
    // 设置背景2锚点为底部中间
    background2_1->setAnchorMode(GameObject::AnchorMode::BottomCentered);

    // 设置游戏对象在主摄像机的中央
    // 背景1始终固定在Ui中, 所以创建一个的UI摄像机
    auto ui_camera =
        std::make_shared<Camera>(Vector2{my_game.getScreenWidth(), my_game.getScreenHeight()});
    main_scene->addCamera("scene-ui", ui_camera);
    main_scene->setCenterAnchorPoint("scene-ui", background);
    ui_camera->addRenderObj("background-1");
    main_scene->setCenterAnchorPoint("scene-main", background2_1);

    ImageTool::scale(*background2_1, {3.4f, 3.4f});
    background2_1->translate({0.0f, ImageTool::size(*background).Y / 2.0f});
    // clone两份出来, 继承2_1的全部属性
    auto background2_2 = background2_1->clone();
    auto background2_3 = background2_1->clone();
    background2_2->translate({-ImageTool::size(*background2_2).X, 0});
    background2_3->translate({ImageTool::size(*background2_2).X, 0});

    // background2 更新逻辑
    background2->addUpdateCallback([](GameObject* _this) {
        auto& scene_manager = SceneManager::getInstance();
        auto& game_af = GameAF::getInstance();

        // 2_1 2_2 2_3
        static auto b2_main = _this->getChildObject(0).get();
        static auto b2_left = _this->getChildObject(1).get();
        static auto b2_right = _this->getChildObject(2).get();

        auto [s_w, s_h] = game_af.getScreenSize();
        // 获取得到player的游戏对象
        auto player = scene_manager.getScene("main-scene")->getGameObjects("Player")[0];
        // 右边情况
        if (player->getPosition().X + s_w / 2.0 >=
            b2_right->getPosition().X + ImageTool::size(*b2_right).X / 2) {
            gameaf::log("left{}->right", b2_left->getPosition());
            b2_left->setPositionX(b2_right->getPosition().X + ImageTool::size(*b2_right).X);
            auto temp = b2_right;
            b2_right = b2_left;
            b2_left = b2_main;
            b2_main = temp;
        }
        // 左边情况
        if (player->getPosition().X - s_w / 2.0 <=
            b2_left->getPosition().X - ImageTool::size(*b2_left).X / 2) {
            gameaf::log("right{}->left", b2_right->getPosition());
            b2_right->setPositionX(b2_left->getPosition().X - ImageTool::size(*b2_left).X);
            auto temp = b2_left;
            b2_left = b2_right;
            b2_right = b2_main;
            b2_main = temp;
        }
    });

    // 玩家游戏对象
    auto player = std::make_shared<Player>();
    main_scene->setCenterAnchorPoint("scene-main", player);
    // 主场景添加游戏对象
    main_scene->addGameObjects({background2, background, player});
    auto main_camera = main_scene->getCamera("scene-main");
    // 主摄像机聚焦player
    main_camera->setFollowTarget(player);
    main_camera->addRenderObj("GameObject");
    main_camera->addRenderObj("Player");

    gameaf::log("{}", "游戏开始运行...");
    my_game.run();
}