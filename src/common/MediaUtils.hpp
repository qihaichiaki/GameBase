#pragma once

#include <scene/Camera.h>

#include <common/Log.hpp>
#include <game_object/component/TImage.hpp>

#include "Common.h"
#include "macros.h"

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
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static BLENDFUNCTION blendFunc = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    int dst_x = static_cast<int>(dst.x - camera.GetPosition().X);
    int dst_y = static_cast<int>(dst.y - camera.GetPosition().Y);
    // gameaf::log("渲染位置:{},{}", dst_x, dst_y);
    AlphaBlend(GetImageHDC(GetWorkingImage()), dst_x, dst_y, dst.w, dst.h,
               GetImageHDC(img->GetSrc()), src.x, src.y, src.w, src.h, blendFunc);
#else
// todo.....
#endif
}

/// @brief 加载字体
/// @param path 字体路径
/// @note 注意后续字体的名字就是后缀.ttf前面的name
inline void LoadFont(const std::string& path)
{
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    AddFontResourceEx(path.c_str(), FR_PRIVATE, nullptr);
#else
#endif
}

inline void UnLoadFont(const std::string& path)
{
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    RemoveFontResourceEx(path.c_str(), FR_PRIVATE, nullptr);
#else
#endif
}

/// @brief 设置字体大小
/// @param fontName 字体名
/// @param size 像素大小
inline void SetFontSize(const std::string& fontName, int size)
{
#ifdef GAMEAF_USE_EASYX
    settextstyle(size, 0, fontName.c_str());
#else
#endif
}

inline int GetTextWidth(char c)
{
#ifdef GAMEAF_USE_EASYX
    return textwidth(c);
#else
#endif
}

inline int GetTextHeight(char c)
{
#ifdef GAMEAF_USE_EASYX
    return textheight(c);
#else
#endif
}

// 字体缓存
// static std::unordered_map<std::string, std::wstring> textCache;

/// @brief 绘制字体及其阴影
/// @param camera 摄像机对象
/// @param text 文本内容
/// @param position 左上角坐标
/// @param textRGB 文本的颜色(默认白色)
/// @param textShaded 文本阴影的颜色(默认灰色)
inline void PutTextShaded(const Camera& camera, const std::string& text, const Vector2& position,
                          const ColorRGB& textRGB = ColorRGB{},
                          const ColorRGB& textShadedRGB = ColorRGB{45, 45, 45})
{
#ifdef GAMEAF_USE_EASYX
    int dst_x = static_cast<int>(position.X - camera.GetPosition().X);
    int dst_y = static_cast<int>(position.Y - camera.GetPosition().Y);

    //     // 将string 转换为 wstring, 拟增加text 缓存, 切换场景时清除缓存?
    //     if (textCache.count(text) == 0) {
    // #ifdef _MSC_VER
    //         int size_needed = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), (int)text.size(),
    //         NULL, 0); std::wstring wtext(size_needed, 0); MultiByteToWideChar(CP_UTF8, 0,
    //         text.c_str(), (int)text.size(), &wtext[0], size_needed); textCache.emplace(text,
    //         wtext);
    // #else
    // #endif
    //     }

    // 绘制文本阴影
    settextcolor(RGB(textShadedRGB.r, textShadedRGB.g, textShadedRGB.b));
    outtextxy(dst_x + 3, dst_y + 3, text.c_str());
    // 绘制主文本
    settextcolor(RGB(textRGB.r, textRGB.g, textRGB.b));
    outtextxy(dst_x, dst_y, text.c_str());
#else
#endif
}

}  // namespace gameaf