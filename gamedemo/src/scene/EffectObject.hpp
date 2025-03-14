#pragma once

#include <GameAf.h>
#include <game_object/GameObject.h>
#include <game_object/component/Animator.h>

#include <common/Log.hpp>

/**
 * 效果动画控制对象
 */

using gameaf::Animator;
using gameaf::GameObject;
using gameaf::log;
using gameaf::Vector2;

class Bug : public GameObject
{
public:
    void OnAwake() override
    {
        // 添加动画组件, bug
        CreateComponent<Animator>()->AddAnimationForAtlas("idle", "bug");
    }

    GameObject::GameObjectPtr Clone() override
    {
        auto clonePtr = std::make_shared<Bug>(*this);
        if (auto parent = GetParent()) {
            parent->AddChildObject(clonePtr);
        }
        return clonePtr;
    }

    void OnUpdate() override
    {
        if (isStart) {
            // 增加效果，在以自身为中心的[100, 300]随机 的矩形框内随机移动
            int x = GameAF::GetInstance().Random(100, 300);
            int y = GameAF::GetInstance().Random(100, 300);
            size = {x * 1.0f, y * 1.0f};
            position = GetPosition();
            target = {
                GameAF::GetInstance().Random(position.X - size.X / 2, position.X + size.X / 2) *
                    1.0f,
                GameAF::GetInstance().Random(position.Y - size.Y / 2, position.Y + size.Y / 2) *
                    1.0f};
            dir = (target - position).Normalized();
            if (GameAF::GetInstance().Random(0, 100) >= 50) {
                // 快
                speed = GameAF::GetInstance().Random(0.15f, 0.25f);
                GetComponent<Animator>()->GetCurrentAnimation().SetInterval(
                    GameAF::GetInstance().Random(0.08f, 0.12f));
            } else {
                // 慢
                speed = GameAF::GetInstance().Random(0.07f, 0.15f);
                GetComponent<Animator>()->GetCurrentAnimation().SetInterval(
                    GameAF::GetInstance().Random(0.13f, 0.20f));
            }
            isStart = false;
        }

        Vector2 currentPos = GetPosition();
        if ((target - currentPos).Length() < (dir * speed).Length()) {
            // 随机位置
            target = {
                GameAF::GetInstance().Random(position.X - size.X / 2, position.X + size.X / 2) *
                    1.0f,
                GameAF::GetInstance().Random(position.Y - size.Y / 2, position.Y + size.Y / 2) *
                    1.0f};
            dir = (target - currentPos).Normalized();
        }

        Translate(dir * speed);
    }

private:
    Vector2 size;
    Vector2 position;
    Vector2 target;
    Vector2 dir;
    float speed;
    bool isStart = true;
};