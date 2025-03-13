#pragma once

namespace gameaf {
class StateNode
{
public:
    StateNode() = default;
    virtual ~StateNode() = default;

public:
    virtual void OnEnter() {};
    virtual void OnUpdate() {};
    virtual void OnExit() {};
};
}  // namespace gameaf