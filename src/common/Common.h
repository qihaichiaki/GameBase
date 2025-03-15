#pragma once

#include <string>
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
using word = unsigned short;
using dword = unsigned long;
struct ColorRGB
{
    byte r = 255, g = 255, b = 255;

    ColorRGB() = default;
    ColorRGB(byte r, byte g, byte b) : r(r), g(g), b(b) {}
    /// @brief 解析二进制颜色
    /// @param hex "#FFFFFF"
    ColorRGB(const std::string& hex);

    operator unsigned long() const
    {
        return (((byte)(r) | ((word)((byte)(g)) << 8)) | (((dword)(byte)(b)) << 16));
    }
};

}  // namespace gameaf