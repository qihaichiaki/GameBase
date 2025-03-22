#include "HornetStateNodes.h"

#include <game_object/component/Animator.h>

#include "../player/Player.h"

namespace hornet {
HornetStateNode::HornetStateNode(Hornet* hornet)
    : hornet(hornet), animator(hornet->GetComponent<Animator>())
{
}

// === sit ===
void Sit::OnEnter() { animator->SwitchToAnimation("sit"); }

void Sit::OnUpdate()
{
    if (hornet->PlayerInRange(hornet->searchPlayerRange)) {
        hornet->SwitchState("Idle");
    }
}

// === Idle ===

void Idle::OnEnter() { animator->SwitchToAnimation("idle"); }

void Idle::OnUpdate()
{
    if (!hornet->PlayerInRange(hornet->searchPlayerRange)) {
        hornet->SwitchState("Sit");
    }
}
}  // namespace hornet