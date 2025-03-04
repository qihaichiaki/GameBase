#pragma once

/**
 * 这里表示碰撞的层级/碰撞组件的类型
 */

namespace gameaf {

using Collisionlayer = int;

enum class CollisionType { None, Box };

struct CollisionLayerTool
{
    constexpr static Collisionlayer none = 0;
    constexpr static Collisionlayer player = Collisionlayer(1 << 0);
    constexpr static Collisionlayer enemy = Collisionlayer(1 << 1);
    constexpr static Collisionlayer wall = Collisionlayer(1 << 2);

    /// @brief 检查src层级是否包含dst层级
    static bool IsContain(Collisionlayer src, Collisionlayer dst) { return src & dst; }

    /// @brief 使src层级包含dst层级
    static void Involve(Collisionlayer& src, Collisionlayer dst) { src |= dst; }
};

}  // namespace gameaf