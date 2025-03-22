#pragma once

/**
 * background 对象
 * 用来控制游戏场景中背景的移动
 */

#include <GameAf.h>
#include <game_object/GameObject.h>
#include <game_object/component/CollisionBox.h>
#include <game_object/component/Image.h>

#include "../Common.hpp"

using namespace gameaf;

// background 底部
// 固定在UI相机
class BackgroundBottom : public GameObject
{
public:
    BackgroundBottom() : GameObject(RenderZOrder::BACKGROUND_1, "BackgroundBottom") {}

    void OnAwake() override
    {
        CreateComponent<Image>(std::string{"background_layer1"})->SetSizeScale({4.0f, 4.0f});
    }
};

// background 中部
// 属于主相机渲染
class BackgroundMiddle : public GameObject
{
public:
    BackgroundMiddle(GameObject* player)
        : GameObject(RenderZOrder::BACKGROUND_2, "BackgroundMiddle"), player(player)
    {
    }

    void OnAwake() override
    {
        s_w = GameAf::GetScreenWidth() * 1.0f;

        auto middle1 = std::make_shared<GameObject>("_BackgroundMiddle");
        auto img = middle1->CreateComponent<Image>(std::string{"background_layer2"});
        img->SetSizeScale({4.0f, 4.0f});
        img->SetAnchorMode(ImageAnchorMode::BottomCentered);
        middle1->Translate({0.0f, GameAf::GetScreenHeight() * 1.0f / 2});
        AddChildObject(middle1);
        center = middle1.get();
        left = middle1->Clone().get();
        right = middle1->Clone().get();

        left->Translate({-s_w, 0.0f});
        right->Translate({s_w, 0.0f});
        lastPos = player->GetPosition();
    }

    void OnUpdate() override
    {
        Vector2 currentPos = player->GetPosition();
        if (lastPos != currentPos) {
            lastPos = currentPos;
            // 右边情况
            if (currentPos.X >= right->GetPosition().X) {
                left->SetPositionX(right->GetPosition().X + s_w);
                auto temp = right;
                right = left;
                left = center;
                center = temp;
            }
            // 左边情况
            if (currentPos.X <= left->GetPosition().X) {
                right->SetPositionX(left->GetPosition().X - s_w);
                auto temp = left;
                left = right;
                right = center;
                center = temp;
            }
        }
    }

private:
    GameObject* player;
    GameObject* left;
    GameObject* center;
    GameObject* right;
    Vector2 lastPos;
    float s_w;
};

// background 表面 - 地皮
// 属于主相机渲染
class Ground : public GameObject
{
public:
    Ground() : GameObject(RenderZOrder::GROUND, "Ground") {}

    void OnAwake() override
    {
        auto img = CreateComponent<Image>(std::string{"ground"});
        img->SetSizeScale({2.08f, 0.84f});
        img->SetAnchorMode(ImageAnchorMode::BottomCentered);
        Translate({0.0f, GameAf::GetScreenHeight() * 0.5f + 150.0f});  //

        collisionBox = CreateComponent<CollisionBox>(
            Vector2{15.0f, -img->GetSize().Y / 2 + 10.0f});  //给 Ground 创建碰撞box, 防止玩家掉下去
        collisionBox->SetSize({collisionBox->GetSize().X - 180.0f, collisionBox->GetSize().Y});
        collisionBox->SetSrcLayer(CollisionLayerTool::wall);
        collisionBox->AddDstLayer(CollisionLayerTool::player |
                                  CollisionLayerTool::enemy);  // 目标包含player、enemy对象
    }

    void OnDraw(const Camera& camera) override
    {
        if (isDebug) collisionBox->OnDebugRender(camera);
    }

public:
    CollisionBox* collisionBox;
    bool isDebug = false;
};

class Wall : public GameObject
{
public:
    Wall() : GameObject(RenderZOrder::WALL, "Wall") {}

    void OnAwake() override
    {
        auto img = CreateComponent<Image>(std::string{"wall"});
        // img->SetSizeScale({2.08f, 0.84f});
        img->SetAnchorMode(ImageAnchorMode::TopCentered);
        // Translate({0.0f, GameAf::GetScreenHeight() * 0.5f + 150.0f});  //

        collisionBox = CreateComponent<CollisionBox>(Vector2{0.0f, img->GetSize().Y / 2});
        collisionBox->SetSize({collisionBox->GetSize().X, collisionBox->GetSize().Y - 10.0f});
        collisionBox->SetSrcLayer(CollisionLayerTool::wall);
        collisionBox->AddDstLayer(CollisionLayerTool::player |
                                  CollisionLayerTool::enemy);  // 目标包含player、enemy对象
    }

    void OnDraw(const Camera& camera) override
    {
        if (isDebug) collisionBox->OnDebugRender(camera);
    }

public:
    CollisionBox* collisionBox;
    bool isDebug = false;
};