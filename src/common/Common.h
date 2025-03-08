#pragma once

/**
 * 存放一些公共的类型声明
 */

namespace gameaf {

// 矩形, 用来标识渲染矩形的位置和大小
struct Rect
{
    float x, y;
    int w, h;
};

using byte = unsigned char;
struct ColorRGB
{
    byte r = 255, g = 255, b = 255;
};

}  // namespace gameaf