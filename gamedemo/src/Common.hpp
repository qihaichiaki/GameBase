#pragma once

/**
 * 游戏公共头文件
 */

struct RenderZOrder
{
    constexpr static int BACKGROUND = -1;
    constexpr static int PLAYER = 0;
    constexpr static int UI_1 = 1;
    constexpr static int UI_2 = 2;
};