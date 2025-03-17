#pragma once

/**
 * 游戏公共头文件
 */

struct RenderZOrder
{
    constexpr static int BACKGROUND_1 = -3;
    constexpr static int BACKGROUND_2 = -2;  // 背景划分两个个区域
    constexpr static int GROUND = -1;        // 地面
    constexpr static int PLAYER = 0;
    constexpr static int UI_1 = 1;
    constexpr static int UI_2 = 2;  // UI划分两个区域
};