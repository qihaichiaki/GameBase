#pragma once

#include "../include/vector2.hpp"

namespace gameaf {
class Camera
{
public:
    Camera() = default;
    Camera(Vector2 size) : m_size(size) {}
    ~Camera() = default;

    Vector2 getPosition() const { return m_position; }
    void setPosition(Vector2 position) { m_position = position; }

    Vector2 getSize() const { return m_size; }
    void setSize(Vector2 size) { m_size = size; }

    /// @brief 摄像机追随位置
    void lookAt(Vector2 position) { m_position = position - m_size / 2; }

private:
    Vector2 m_position;  // 相机的左上角的世界坐标
    Vector2 m_size;      // 相机的大小(宽高)
};

}  // namespace gameaf