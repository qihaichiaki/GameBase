#include "Enemy.h"

#include "../player/Player.h"

bool Enemy::PlayerInRange(float distance) const
{
    return (GetPosition() - player->GetPosition()).Length() <= distance;
}