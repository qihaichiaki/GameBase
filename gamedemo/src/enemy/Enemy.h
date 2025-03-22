#pragma once

#include "../Character.hpp"
#include "../Common.hpp"

/**
 * 敌人控制对象
 */

class Player;

class Enemy : public Character
{
public:
    Enemy(const std::string& name, Player* player)
        : Character(RenderZOrder::ENEMY, name), player(player)
    {
    }

public:
    bool PlayerInRange(float distance) const;

public:
    Player* player;
};