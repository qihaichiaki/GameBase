#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "state_node.hpp"

namespace gameaf {
class StateMachine
{
public:
    using StatePtr = std::shared_ptr<StateNode>;

    // 设置初始状态
    void setEntry(const std::string& id) { m_currentState = m_statePool.at(id); }
    // 切换状态
    void switchTo(const std::string& id)
    {
        m_currentState->onExit();
        m_currentState = m_statePool.at(id);
        m_currentState->onEnter();
    }
    // 注册状态
    void registerState(const std::string& id, StatePtr stateNode)
    {
        m_statePool[id] = std::move(stateNode);
    }

protected:
    bool m_needInit = true;
    StatePtr m_currentState = nullptr;
    std::unordered_map<std::string, StatePtr> m_statePool;
};
}  // namespace gameaf