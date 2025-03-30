#include "Enemy.h"

bool Enemy::PlayerInRange(float distance) const
{
    return (GetPosition() - dstEnemy->GetPosition()).Length() <= distance;
}