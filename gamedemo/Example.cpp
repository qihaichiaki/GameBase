#include <GameAf.h>
#include <game_object/GameObject.h>
#include <game_object/component/Animator.h>
#include <game_object/component/AudioManager.h>
#include <game_object/component/CollisionBox.h>
#include <game_object/component/CollisionRaycaster.h>
#include <game_object/component/Image.h>
#include <game_object/component/Rigidbody2D.h>
#include <game_object/component/Text.h>
#include <input/InputManager.h>
#include <resource/ResourceManager.h>
#include <scene/Camera.h>
#include <scene/Scene.h>
#include <scene/SceneManager.h>

using gameaf::Animation;
using gameaf::Animator;
using gameaf::AudioManager;
using gameaf::Camera;
using gameaf::Collision;
using gameaf::CollisionBox;
using gameaf::CollisionLayerTool;
using gameaf::CollisionRaycaster;
using gameaf::ColorRGB;
using gameaf::GameObject;
using gameaf::Image;
using gameaf::ImageAnchorMode;
using gameaf::InputManager;
using gameaf::RaycasterDir;
using gameaf::ResourceManager;
using gameaf::Rigidbody2D;
using gameaf::Scene;
using gameaf::SceneManager;
using gameaf::Text;
using gameaf::TextAlignMode;
using gameaf::Vector2;

enum ZOrderLevel { z_background_back, z_background_front, z_player };

class BackGround : public GameObject
{
public:
    BackGround(int z_redner = ZOrderLevel::z_background_back) : GameObject(z_redner) {}
    BackGround(const std::string& image_id, int z_redner = ZOrderLevel::z_background_back)
        : GameObject(z_redner)
    {
        img = CreateComponent<Image>(image_id);
    }
    BackGround(const BackGround&) = default;

    Image* img;
};

class Player : public GameObject
{
public:
    Player()
    {
        SetName("Player");
        SetZOrder(ZOrderLevel::z_player);
        // 创建animator组件
        animator = CreateComponent<Animator>();

        auto& resourceManager = ResourceManager::GetInstance();

        animator->AddAnimation("idle", resourceManager.GetAtlas("player-idle"));
        animator->AddAnimation("run", resourceManager.GetAtlas("player-run"));
        animator->AddAnimation("jump", resourceManager.GetAtlas("player-jump"), false);
        animator->AddAnimation("fall", resourceManager.GetAtlas("player-fall"));

        collisionBox = CreateComponent<CollisionBox>();
        collisionBox->SetSize({50.0f, collisionBox->GetSize().Y});
        collisionBox->SetSrcLayer(CollisionLayerTool::player);
        collisionBox->AddDstLayer(CollisionLayerTool::wall);
        collisionBox->SetOnCollide([this](Collision*) {});
        rigidbody2D = CreateComponent<Rigidbody2D>();
        // rigidbody2D->gravityScale() = 10.5f;

        // 创建文本组件
        auto text = CreateComponent<Text>(std::wstring{L"zpix"});
        // text->SetOffset({-collisionBox->GetSize().X, -collisionBox->GetSize().Y / 2});
        text->SetTextBoxSize({0.0f, collisionBox->GetSize().Y});
        text->SetAlignMode(TextAlignMode::CenterTop);
        text->SetFontSize(20);
        text->SetText(L"技术宅拯救世界");
        text->SetTextColor(ColorRGB{229, 164, 100});

        // 创建射线检测组件
        ray = CreateComponent<CollisionRaycaster>();
        ray->SetSrcLayer(CollisionLayerTool::player);
        ray->AddDstLayer(CollisionLayerTool::wall);
        ray->SetDir(RaycasterDir::Down);
        ray->SetLength(80.0f);

        // 获取主相机
        mainCamera =
            SceneManager::GetInstance().GetScene("main-scene")->GetCamera("scene-main").get();
    }

    void OnUpdate() override
    {
        using KeyValue = InputManager::KeyValue;
        auto& input_manager = InputManager::GetInstance();
        auto& game_af = GameAF::GetInstance();
        auto& audioManager = AudioManager::GetInstance();

        // 测试音量
        if (input_manager.GetKey(KeyValue::_1)) {
            if (input_manager.IsKeyDown(KeyValue::OEM_PLUS)) {
                // 1 +
                audioManager.AdjustRelativeVolume("bgm-start", 0.1f);
                gameaf::log("增加了bgm-start的音量, 音量强度:{}",
                            audioManager.GetVolume("bgm-start"));
            }
            if (input_manager.IsKeyDown(KeyValue::OEM_MINUS)) {
                // 1 -
                audioManager.AdjustRelativeVolume("bgm-start", -0.1f);
                gameaf::log("减少了bgm-start的音量, 音量强度:{}",
                            audioManager.GetVolume("bgm-start"));
            }
        } else if (input_manager.GetKey(KeyValue::_2)) {
            if (input_manager.IsKeyDown(KeyValue::OEM_PLUS)) {
                // 1 +
                audioManager.AdjustRelativeVolume("bullet_time", 0.1f);
                gameaf::log("增加了bullet_time的音量, 音量强度:{}",
                            audioManager.GetVolume("bullet_time"));
            }
            if (input_manager.IsKeyDown(KeyValue::OEM_MINUS)) {
                // 1 -
                audioManager.AdjustRelativeVolume("bullet_time", -0.1f);
                gameaf::log("减少了bullet_time的音量, 音量强度:{}",
                            audioManager.GetVolume("bullet_time"));
            }
        } else if (input_manager.IsKeyDown(KeyValue::OEM_PLUS)) {
            audioManager.AdjustGlobalVolume(0.1f);
            gameaf::log(
                "增加了全局的音量, 总音量强度:{}\nbgm-start的音量强度:{}\nbullet_time的音量强度:{}",
                audioManager.GetGlobalVolume(), audioManager.GetVolume("bgm-start"),
                audioManager.GetVolume("bullet_time"));
        } else if (input_manager.IsKeyDown(KeyValue::OEM_MINUS)) {
            audioManager.AdjustGlobalVolume(-0.1f);
            gameaf::log(
                "减少了全局的音量, 总音量强度:{}\nbgm-start的音量强度:{}\nbullet_time的音量强度:{}",
                audioManager.GetGlobalVolume(), audioManager.GetVolume("bgm-start"),
                audioManager.GetVolume("bullet_time"));
        }

        // 测试shake
        if (input_manager.IsKeyDown(KeyValue::R)) {
            mainCamera->Shake(0.15f, 15.0f);
            audioManager.PlayAudio("bullet_time");
        }

        // 开发者模式
        if (input_manager.IsKeyDown(KeyValue::K)) {
            // 无限跳
            isK = !isK;
            if (isK) {
                gameaf::log("开发者模式已开: 1. 无限跳跃");
            } else {
                gameaf::log("开发者模式关闭");
            }
        }

        if (input_manager.GetKey(KeyValue::A)) {
            dir = -1.0f;
            if (!is_dir_left) Flip();
            is_dir_left = true;
        } else if (input_manager.GetKey(KeyValue::D)) {
            dir = 1.0f;
            if (is_dir_left) Flip();
            is_dir_left = false;
        } else {
            dir = 0.0f;
        }

        if (input_manager.IsKeyDown(KeyValue::Space)) {
            if (isK || ray->IsCollided() || jumpNum > 0) {
                is_jump = true;
                --jumpNum;
            }
        } else if (ray->IsCollided()) {
            jumpNum = 2;
        }

        rigidbody2D->Velocity() = {dir * speed, is_jump ? -600.0f : rigidbody2D->Velocity().Y};
        const Vector2& v = rigidbody2D->Velocity();

        if (v.Y > 0) {
            animator->SwitchToAnimation("fall");
        }
        if (v.Y < 0) {
            animator->SwitchToAnimation("jump");
        }

        if (v.Y == 0) {
            if (std::abs(v.X) < 1e-5) {
                animator->SwitchToAnimation("idle");
            } else {
                animator->SwitchToAnimation("run");
            }
        }

        is_jump = false;
    }

    void OnDraw(const Camera& camera) override { collisionBox->OnDebugRender(camera); }

private:
    Animator* animator;
    CollisionBox* collisionBox;
    Rigidbody2D* rigidbody2D;
    Camera* mainCamera;
    CollisionRaycaster* ray;
    float speed = 500.0f;
    bool is_left = false;
    bool is_right = false;
    bool is_jump = false;
    float dir = -1;
    bool is_dir_left = true;
    int jumpNum = 2;
    bool isK = false;
};

int main()
{
    auto& my_game = GameAF::GetInstance();
    auto& resource_manager = ResourceManager::GetInstance();
    auto& scene_manager = SceneManager::GetInstance();
    auto& audioManager = AudioManager::GetInstance();

    // my_game.SetFPS(240);
    my_game.SetShowConsole(true);
    my_game.InitWindow();
    gameaf::log("{}", "游戏开始预加载设置...");
    gameaf::log("{}", "游戏开始加载图像资源...");

    // <=======资源加载======>
    resource_manager.NewImage(
        ASSETS_PATH "other/Surroundings/Medieval_Castle/Background/layer_1.png", "background-1");
    resource_manager.NewImage(
        ASSETS_PATH "other/Surroundings/Medieval_Castle/Background/layer_2.png", "background-2");

    // 加载idle图集
    resource_manager.LoadAtlas(ASSETS_PATH "kongdongwushi/enemy/idle/%d.png", 6, "player-idle");
    resource_manager.LoadAtlas(ASSETS_PATH "kongdongwushi/enemy/run/%d.png", 8, "player-run");
    resource_manager.LoadAtlas(ASSETS_PATH "kongdongwushi/enemy/jump/%d.png", 8, "player-jump");
    resource_manager.LoadAtlas(ASSETS_PATH "kongdongwushi/enemy/fall/%d.png", 4, "player-fall");

    // 加载音频资源
    resource_manager.LoadAudio(ASSETS_PATH "kongdongwushi/audio/bgm_start.mp3", "bgm-start");
    resource_manager.LoadAudio(ASSETS_PATH "kongdongwushi/audio/bullet_time.mp3", "bullet_time");

    // 加载字体资源
    resource_manager.LoadFont(ASSETS_PATH "kongdongwushi/font/zpix.ttf");

    // <=======资源加载======>

    gameaf::log("游戏开始加载场景资源......");
    scene_manager.Register("main-scene", std::make_shared<Scene>());
    scene_manager.SetEntry("main-scene");
    gameaf::log("游戏开始加载场景中游戏对象资源......");
    auto main_scene = scene_manager.GetScene("main-scene");
    auto background = std::make_shared<BackGround>("background-1");
    background->img->SetSize(background->img->GetSize() * 4.0f);
    background->SetName("background-1");

    // 背景2由一个大的游戏对象, 三个小的游戏对象组成
    auto background2 = std::make_shared<BackGround>(ZOrderLevel::z_background_front);
    auto background2_1 = std::make_shared<BackGround>("background-2");
    background2->AddChildObject(background2_1);

    // background2
    // 设置背景2锚点为底部中间
    background2_1->GetComponent<Image>()->SetAnchorMode(ImageAnchorMode::BottomCentered);

    // 设置游戏对象在主摄像机的中央
    // 背景1始终固定在Ui中, 所以创建一个的UI摄像机
    auto ui_camera =
        std::make_shared<Camera>(Vector2{my_game.GetScreenWidth(), my_game.GetScreenHeight()});
    ui_camera->SetRenderMod(false);
    main_scene->AddCamera("scene-ui", ui_camera);
    main_scene->SetCenterAnchorPoint("scene-ui", background);
    ui_camera->AddRenderObj("background-1");
    // 每个场景存在一个默认的主相机scene-main
    main_scene->SetCenterAnchorPoint("scene-main", background2_1);

    background2_1->img->SetSize(background2_1->img->GetSize() * 3.4f);
    background2_1->Translate({0.0f, background2_1->img->GetSize().Y / 2.0f});
    // clone两份出来, 继承2_1的全部属性
    auto background2_2 = background2_1->Clone();
    auto background2_3 = background2_1->Clone();
    background2_2->Translate({-background2_2->GetComponent<Image>()->GetSize().X, 0});
    background2_3->Translate({background2_2->GetComponent<Image>()->GetSize().X, 0});

    // background2 更新逻辑
    background2->AddUpdateCallback([](GameObject* _this) {
        auto& scene_manager = SceneManager::GetInstance();
        auto& game_af = GameAF::GetInstance();

        // 2_1 2_2 2_3
        static auto b2_main = _this->GetChildObject(0).get();
        static auto b2_left = _this->GetChildObject(1).get();
        static auto b2_right = _this->GetChildObject(2).get();

        auto [s_w, s_h] = game_af.GetScreenSize();
        // 获取得到player的游戏对象
        auto player = scene_manager.GetScene("main-scene")->GetGameObjects("Player")[0];
        // 右边情况
        if (player->GetPosition().X + s_w / 2.0 >=
            b2_right->GetPosition().X + b2_right->GetComponent<Image>()->GetSize().X / 2) {
            gameaf::log("left{}->right", b2_left->GetPosition());
            b2_left->SetPositionX(b2_right->GetPosition().X +
                                  b2_right->GetComponent<Image>()->GetSize().X);
            auto temp = b2_right;
            b2_right = b2_left;
            b2_left = b2_main;
            b2_main = temp;
        }
        // 左边情况
        if (player->GetPosition().X - s_w / 2.0 <=
            b2_left->GetPosition().X - b2_left->GetComponent<Image>()->GetSize().X / 2) {
            gameaf::log("right{}->left", b2_right->GetPosition());
            b2_right->SetPositionX(b2_left->GetPosition().X -
                                   b2_left->GetComponent<Image>()->GetSize().X);
            auto temp = b2_left;
            b2_left = b2_right;
            b2_right = b2_main;
            b2_main = temp;
        }
    });

    // 玩家游戏对象
    auto player = std::make_shared<Player>();
    // 设置玩家对象在当前场景的主相机的中心位置
    main_scene->SetCenterAnchorPoint("scene-main", player);

    // 添加空气墙
    auto air_wall = std::make_shared<GameObject>();
    air_wall->SetName("air_wall");
    air_wall->SetZOrder(ZOrderLevel::z_background_front);
    air_wall->Translate({0.0f, 400.0f});
    auto air_wall_collision = air_wall->CreateComponent<CollisionBox>();
    air_wall_collision->SetSize({500.0f, 500.0f});
    air_wall_collision->SetSrcLayer(CollisionLayerTool::wall);
    air_wall_collision->AddDstLayer(CollisionLayerTool::player);
    auto air_wall_collision2 = air_wall->CreateComponent<CollisionBox>();
    air_wall_collision2->SetSize({20.0f, 500.0f});
    air_wall_collision2->SetSrcLayer(CollisionLayerTool::wall);
    air_wall_collision2->AddDstLayer(CollisionLayerTool::player);
    air_wall_collision2->SetOffset({1400.0f, 0.0f});

    // 空气墙上添加文本组件
    auto air_wall_text = air_wall->CreateComponent<Text>(std::wstring{L"zpix"});
    // air_wall_text->SetAlignMode(TextAlignMode::CenterTop);
    air_wall_text->SetTextBoxSize({500.0f, 500.0f});
    // air_wall_text->SetText(L"你好世界\n");
    air_wall_text->SetText(
        L"This game framework is created by qihai, currently in the testing phase of the text "
        "component. If you're lucky enough to see this text, please feel free to give me more "
        "suggestions. Thank you very much! Love from China~\nI am currently testing my text "
        "widget. As you can see, the text component has a black border, and the effects parameters "
        "when creating components are not very good, feeling quite messy. In the future, the text "
        "component will need to fix these bugs, complete a few modes, and optimize the component "
        "creation process.\nI am now testing extreme cases, specifically how the text box behaves "
        "if the text exceeds its bounds. If you have a better implementation or would like to "
        "share suggestions, please feel free to contact me. You can email me or submit suggestions "
        "on GitHub! I would greatly appreciate it if you could!");

    // 添加地板砖
    auto ground = std::make_shared<GameObject>();
    ground->SetName("ground");
    ground->SetZOrder(ZOrderLevel::z_player);
    main_scene->SetCenterAnchorPoint("scene-main", ground);
    ground->SetPositionY(background2_1->GetPosition().Y - 25.0f);
    auto ground_collision = ground->CreateComponent<CollisionBox>();
    ground_collision->SetSize({1500.0f, 50.0f});
    ground_collision->SetSrcLayer(CollisionLayerTool::wall);
    ground_collision->AddDstLayer(CollisionLayerTool::player);
    auto groundText = ground->CreateComponent<Text>(std::wstring{L"zpix"});
    groundText->EnableShadow(true);
    groundText->SetTextBoxSize({1500.0f, 50.0f});
    groundText->SetAlignMode(TextAlignMode::RightMiddle);
    groundText->SetText(L"作者: 柒海啦 项目还请多支持哦~");

    // 主场景添加游戏对象
    main_scene->AddGameObjects({background2, background, player, air_wall, ground});
    auto main_camera = main_scene->GetCamera("scene-main");
    // main_camera->SetRenderMod(true);
    main_camera->SetPosition({0.0, -200.0f});  // 目的, 隐藏图片背景下的空隙
    // 主摄像机聚焦player
    main_camera->SetFollowTarget(player, Camera::FollowMode::Smooth);
    // 设置摄像机死区
    main_camera->SetCameraDeadZone({200.0f, 500.0f});

    gameaf::log("{}", "游戏开始运行...");
    // audioManager.PlayAudio("bgm-start", true);
    gameaf::log("bgm-start的音量强度:{}", audioManager.GetVolume("bgm-start"));

    main_scene->SetDebugRenderCamera(true);
    my_game.Run();  // 阻塞死循环

    return 0;
}