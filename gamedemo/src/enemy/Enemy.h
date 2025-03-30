#pragma once

#include "../Character.h"
#include "../Common.hpp"

/**
 * 敌人控制对象
 */

class Enemy : public Character
{
public:
    Enemy(const std::string& name, Character* dstEnemy)
        : Character(RenderZOrder::ENEMY, name), dstEnemy(dstEnemy)
    {
    }

public:
    bool PlayerInRange(float distance) const;

public:
    Character* dstEnemy;
};