#pragma once

#include <scene/Camera.h>

#include <common/Log.hpp>
#include <game_object/component/TImage.hpp>

#include "Common.h"
#include "Macros.h"

#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
#pragma comment(lib, "MSIMG32.lib")  // AlphaBlend
#endif

/**
 * 媒体工具函数
 * 方便游戏框架渲染图形和设置音频相关
 */

namespace gameaf {

/// @brief 渲染图形
/// @param camera 摄像机对象
/// @param img 图元对象
/// @param dst 渲染的目标世界矩形(最终会和摄像机的位置进行转换为视口位置)
/// @param src 图像对象的渲染矩形
inline void PutImageEx(const Camera& camera, TImage* img, const Rect& dst, const Rect& src)
{
    const Vector2& cameraPos = camera.GetPosition();
    const Vector2& cameraSize = camera.GetSize();
    int dst_x = std::round(dst.x - cameraPos.X);
    int dst_y = std::round(dst.y - cameraPos.Y);

    // 检测是否完全超出窗口范围
    if (dst_x + dst.w < 0 || dst_x > cameraSize.X || dst_y + dst.h < 0 || dst_y > cameraSize.Y) {
        return;
    }
    // gameaf::log("渲染位置:{},{}", dst_x, dst_y);
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static BLENDFUNCTION blendFunc = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    AlphaBlend(GetImageHDC(GetWorkingImage()), dst_x, dst_y, dst.w, dst.h,
               GetImageHDC(img->GetSrc()), src.x, src.y, src.w, src.h, blendFunc);
#else
// todo.....
#endif
}

/// @brief 加载字体
/// @param path 字体路径
/// @note 注意后续字体的名字就是后缀.ttf前面的name
inline void LoadFont(const std::wstring& path)
{
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    AddFontResourceEx(path.c_str(), FR_PRIVATE, nullptr);
    setbkmode(TRANSPARENT);
#else
#endif
}

inline void UnLoadFont(const std::wstring& path)
{
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    RemoveFontResourceEx(path.c_str(), FR_PRIVATE, nullptr);
#else
#endif
}

/// @brief 设置字体大小
/// @param fontName 字体名
/// @param size 像素大小
inline void SetFontSize(const std::wstring& fontName, int size)
{
#ifdef GAMEAF_USE_EASYX
    settextstyle(size, 0, fontName.c_str());
#else
#endif
}

// 字体宽度
inline int GetTextWidth(wchar_t c)
{
#ifdef GAMEAF_USE_EASYX
    return textwidth(c);
#else
#endif
}

inline int GetTextHeight(wchar_t c)
{
#ifdef GAMEAF_USE_EASYX
    return textheight(c);
#else
#endif
}

/// @brief 绘制字体
/// @param camera 摄像机对象
/// @param text 文本内容
/// @param position 左上角坐标
/// @param textWidth 文本的宽度(优化渲染)
/// @param textHeight 文本的高度(优化渲染)
/// @param textRGB 文本的颜色(默认白色)
inline void PutText(const Camera& camera, const std::wstring& text, const Vector2& position,
                    int textWidth, int textHeight, const ColorRGB& textRGB = ColorRGB{})
{
    const Vector2& cameraPos = camera.GetPosition();
    const Vector2& cameraSize = camera.GetSize();
    int dst_x = std::round(position.X - cameraPos.X);
    int dst_y = std::round(position.Y - cameraPos.Y);

    // 检测是否完全超出窗口范围
    if (dst_x + textWidth < 0 || dst_x > cameraSize.X || dst_y + textHeight < 0 ||
        dst_y > cameraSize.Y)
        return;
#ifdef GAMEAF_USE_EASYX
    // 绘制主文本
    settextcolor(RGB(textRGB.r, textRGB.g, textRGB.b));
    outtextxy(dst_x, dst_y, text.c_str());
#else
#endif
}

/// @brief 绘制字体及其阴影
/// @param camera 摄像机对象
/// @param text 文本内容
/// @param position 左上角坐标
/// @param textWidth 文本的宽度(优化渲染)
/// @param textHeight 文本的高度(优化渲染)
/// @param textRGB 文本的颜色(默认白色)
/// @param textShaded 文本阴影的颜色(默认灰色)
/// @param textShadedOffset 文本阴影的偏移(默认向右向下各自偏移3像素)
inline void PutTextShaded(const Camera& camera, const std::wstring& text, const Vector2& position,
                          int textWidth, int textHeight, const ColorRGB& textRGB = ColorRGB{},
                          const ColorRGB& textShadedRGB = ColorRGB{45, 45, 45},
                          const Vector2& textShadedOffset = {3.0f, 3.0f})
{
    const Vector2& cameraPos = camera.GetPosition();
    const Vector2& cameraSize = camera.GetSize();
    int dst_x = std::round(position.X - cameraPos.X);
    int dst_y = std::round(position.Y - cameraPos.Y);

    // 检测是否完全超出窗口范围
    if (dst_x + textWidth < 0 || dst_x > cameraSize.X || dst_y + textHeight < 0 ||
        dst_y > cameraSize.Y)
        return;
#ifdef GAMEAF_USE_EASYX
    // 绘制文本阴影
    settextcolor(RGB(textShadedRGB.r, textShadedRGB.g, textShadedRGB.b));
    outtextxy(dst_x + textShadedOffset.X, dst_y + textShadedOffset.Y, text.c_str());
    // 绘制主文本
    settextcolor(RGB(textRGB.r, textRGB.g, textRGB.b));
    outtextxy(dst_x, dst_y, text.c_str());
#else
#endif
}

}  // namespace gameaf