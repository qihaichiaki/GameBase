#ifndef __MACRO_H__
#define __MACRO_H__

#if defined(GAMEAF_USE_EASYX)
#include <easyx.h>
#include <graphics.h>
#else
// ...
#endif

// 最大帧率
#define GAMEAF_MAX_FPS (int)260
// 最小帧率
#define GAMEAF_MIN_FPS (int)14

// 设置windows控制台输出/输入编码为 UTF-8
#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS_USE_UTF8         \
    SetConsoleOutputCP(CP_UTF8); \
    SetConsoleCP(CP_UTF8)
#else
#define WINCMD_USE_UTF8
#endif

#endif  // __MACRO_H__