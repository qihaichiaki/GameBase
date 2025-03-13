#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "StateNode.hpp"

namespace gameaf {
class StateMachine
{
public:
    using StatePtr = std::shared_ptr<StateNode>;

    // 设置初始状态
    void SetEntry(const std::string& id) { m_currentState = m_statePool.at(id); }
    // 切换状态
    void SwitchTo(const std::string& id)
    {
        m_currentState->OnExit();
        m_currentState = m_statePool.at(id);
        m_currentState->OnEnter();
    }
    // 注册状态
    void RegisterState(const std::string& id, StatePtr stateNode)
    {
        m_statePool[id] = std::move(stateNode);
        if (m_currentState == nullptr) m_currentState = m_statePool[id];
    }

protected:
    bool m_needInit = true;
    StatePtr m_currentState = nullptr;
    std::unordered_map<std::string, StatePtr> m_statePool;
};
}  // namespace gameaf