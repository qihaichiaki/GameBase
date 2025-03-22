#pragma once

#include "Enemy.h"

/**
 * Boss Horent 敌人ai
 */
class Hornet : public Enemy
{
public:
    Hornet(Player* player) : Enemy("Horent", player) {}

    void OnAwake() override;

    void OnDraw(const Camera& camera) override;

    GameObjectPtr Clone() override
    {
        auto clonePtr = std::make_shared<Hornet>(*this);
        if (auto parent = GetParent()) {
            parent->AddChildObject(clonePtr);
        }
        return clonePtr;
    }

    void OnUpdate() override;

public:
    float searchPlayerRange = 500.0f;  // 索敌范围
};