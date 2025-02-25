#pragma once

#include <camera.h>

#include <log.hpp>
#include <module/image.hpp>

#include "macros.h"

#ifdef _MSC_VER
#pragma comment(lib, "MSIMG32.lib")  // AlphaBlend
#pragma comment(lib, "WINMM.lib")    // 音频播放
#else
#endif

/**
 * 媒体工具函数
 * 方便游戏框架渲染图形和设置音频相关
 */

namespace gameaf {

// 矩形, 通常由左上角位置(二维坐标系下) + 宽高组成
struct Rect
{
    float x, y;
    int w, h;
};

/// @brief 渲染图形
/// @param camera 摄像机对象
/// @param img 图像对象
/// @param dst 渲染的目标世界矩形(最终会和摄像机的位置进行转换为视口位置)
/// @param src 图像对象的渲染矩形
inline void putImageEx(const Camera& camera, Image& img, const Rect& dst, const Rect& src)
{
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static BLENDFUNCTION blendFunc = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    int dst_x = static_cast<int>(dst.x - camera.getPosition().X);
    int dst_y = static_cast<int>(dst.y - camera.getPosition().Y);
    // gameaf::log("渲染位置:{},{}", dst_x, dst_y);
    AlphaBlend(GetImageHDC(GetWorkingImage()), dst_x, dst_y, dst.w, dst.h,
               GetImageHDC(img.getSrc()), src.x, src.y, src.w, src.h, blendFunc);
#else
// todo.....
#endif
}

/// @brief 渲染图形
/// @param camera 摄像机对象
/// @param img 图像对象
/// @param dst 渲染的目标世界矩形(最终会和摄像机的位置进行转换为视口位置)
inline void putImageEx(const Camera& camera, Image& img, const Rect& dst)
{
    putImageEx(camera, img, dst, {0.0f, 0.0f, img.getWidth(), img.getHeight()});
}

/// @brief 加载音频
/// @param path 资源路径
/// @param id 资源id
inline void loadAudio(LPCTSTR path, LPCTSTR id)
{
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static TCHAR strCmd[512];
    _stprintf_s(strCmd, _T("open %s alias %s"), path, id);
    mciSendString(strCmd, NULL, 0, NULL);
#else
#endif
}

/// @brief 播放音频
/// @param id 音频id
/// @param isLoop 是否循环
inline void playAudio(LPCTSTR id, bool isLoop = false)
{
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static TCHAR strCmd[512];
    _stprintf_s(strCmd, _T("play %s %s from 0"), id, isLoop ? _T("repeat") : _T(""));
    mciSendString(strCmd, NULL, 0, NULL);
#else
#endif
}

/// @brief 停止音频
/// @param id 音频id
inline void stopAudio(LPCTSTR id)
{
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static TCHAR strCmd[512];
    _stprintf_s(strCmd, _T("stop %s"), id);
    mciSendString(strCmd, NULL, 0, NULL);
#else
#endif
}

/// @brief 加载字体
/// @param path 字体路径
/// @note 注意后续字体的名字就是后缀.ttf前面的name
inline void loadFont(LPCSTR path)
{
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    AddFontResourceEx(path, FR_PRIVATE, nullptr);
#else
#endif
}

}  // namespace gameaf