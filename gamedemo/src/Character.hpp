#pragma once

#include <game_object/GameObject.h>

#include <common/StateMachine.hpp>

/**
 * 角色基类, 所有角色都共有的属性:
 * 1. hp
 * 2. 状态机
 * ......
 */

using gameaf::GameObject;
using gameaf::StateMachine;

class Character : public GameObject
{
public:
    Character(int zOrder, const std::string& name) : GameObject(zOrder, name) {}

    void OnUpdate() override { m_stateMachine.OnUpdate(); }

public:
    void SwitchState(const std::string& stateId) { m_stateMachine.SwitchTo(stateId); }

protected:
    StateMachine m_stateMachine;
};