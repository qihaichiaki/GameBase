#pragma once
#include <common/StateNode.hpp>

#include "Hornet.h"

/**
 * 大黄蜂npc状态节点
 */

namespace hornet {
class HornetStateNode : public StateNode
{
public:
    HornetStateNode(Hornet* hornet);

protected:
    // 保护属性
    Hornet* hornet;
    Animator* animator;
};

class Sit : public HornetStateNode
{
public:
    Sit(Hornet* hornet) : HornetStateNode(hornet) {}

    void OnEnter() override;
    void OnUpdate() override;
};

// 闲置状态
class Idle : public HornetStateNode
{
public:
    Idle(Hornet* hornet) : HornetStateNode(hornet) {}

    void OnEnter() override;
    void OnUpdate() override;
};
}  // namespace hornet