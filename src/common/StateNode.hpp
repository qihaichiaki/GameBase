#pragma once

namespace gameaf {
class StateNode
{
public:
    virtual void onEnter() {};
    virtual void onUpdate() {};
    virtual void onExit() {};
};
}  // namespace gameaf