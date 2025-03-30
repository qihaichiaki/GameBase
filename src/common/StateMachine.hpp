#pragma once

#include <common/Log.hpp>
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
    void SetEntry(const std::string& id) { m_currentState = m_statePool.at(id).get(); }
    // 切换状态
    void SwitchTo(const std::string& id)
    {
        m_currentState->OnExit();
        if (m_statePool.count(id) == 0) {
            gameaf::log("[error][SwitchTo] 未注册状态{}", id);
            return;
        }
        m_currentState = m_statePool.at(id).get();
        m_currentState->OnEnter();
    }
    // 注册状态
    void RegisterState(const std::string& id, StatePtr stateNode)
    {
        m_statePool[id] = std::move(stateNode);
        if (m_currentState == nullptr) m_currentState = m_statePool[id].get();
    }
    // 更新当前状态
    void OnUpdate() { m_currentState->OnUpdate(); }

    /// @brief 获得当前状态
    StateNode* GetCurrentState() { return m_currentState; }

protected:
    bool m_needInit = true;
    StateNode* m_currentState = nullptr;
    std::unordered_map<std::string, StatePtr> m_statePool;
};
}  // namespace gameaf