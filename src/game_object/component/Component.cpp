#include "Component.h"

#include <game_object/GameObject.h>

#include <common/Log.hpp>

namespace gameaf {
Component::Component(GameObject* obj, const Vector2& offset) : m_gameObject(obj), m_offset(offset)
{
    // if (obj == nullptr) {
    //     gameaf::log("[error][Component] 构造时游戏对象不可为空!");
    // }
}
Component::~Component() {}

Vector2 Component::Position() const
{
    if (m_gameObject) {
        return m_gameObject->GetPosition() + m_offset;
    }
    return m_offset;
}

}  // namespace gameaf