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

}  // namespace gameaf