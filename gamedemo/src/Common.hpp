#pragma once

#include <input/InputManager.h>

/**
 * 游戏公共头文件
 */

// 渲染层级
struct RenderZOrder
{
    constexpr static int BACKGROUND_1 = -4;
    constexpr static int BACKGROUND_2 = -3;  // 背景划分两个个区域
    constexpr static int WALL = -2;          // 墙
    constexpr static int GROUND = -1;        // 地面
    constexpr static int ENEMY = 0;
    constexpr static int PLAYER = 1;
    constexpr static int UI_1 = 2;
    constexpr static int UI_2 = 3;  // UI划分两个区域
};

using gameaf::InputManager;
using KeyValue = InputManager::KeyValue;
// 键盘映射
struct InputKey
{
    static InputManager& input;
    // move 按键映射
    static KeyValue moveLeft;
    static KeyValue moveRight;
    static KeyValue jump;
    static KeyValue crouch;
    static KeyValue roll;
    static KeyValue attack;
    static KeyValue block;

    /// @brief 尝试移动
    static bool TryMove() { return input.GetKey(moveLeft) || input.GetKey(moveRight); }

    /// @brief 尝试跳跃
    static bool TryJump() { return input.IsKeyDown(jump); }

    /// @brief 尝试蹲下
    static bool TryCrouch() { return input.GetKey(crouch); }

    /// @brief 尝试翻滚
    static bool TryRoll() { return input.IsKeyDown(roll); }

    /// @brief 尝试攻击
    static bool TryAttack() { return input.IsKeyDown(attack); }

    /// @brief 尝试格挡
    static bool TryBlock() { return input.GetKey(block); }

    /// @brief 返回水平移动的的方向
    static int GetHorizontalDir()
    {
        return input.GetKey(moveLeft) ? -1 : input.GetKey(moveRight) ? 1 : 0;
    }
};

inline InputManager& InputKey::input = InputManager::GetInstance();
inline KeyValue InputKey::moveLeft = KeyValue::A;
inline KeyValue InputKey::moveRight = KeyValue::D;
inline KeyValue InputKey::jump = KeyValue::Space;
inline KeyValue InputKey::crouch = KeyValue::S;
inline KeyValue InputKey::roll = KeyValue::Shift;
inline KeyValue InputKey::attack = KeyValue::J;
inline KeyValue InputKey::block = KeyValue::K;
