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

/// @brief 旋转图像
/// @param img 待旋转图像
/// @param radian 旋转弧度
/// @return 旋转后的新图像(注意别内存泄露)
inline TImage* RotateImageEx(TImage* img, double radian)
{
    // 目前先不支持精灵图的旋转
    if (img->GetSpriteNum() > 1) return nullptr;

#ifdef GAMEAF_USE_EASYX
    radian = -radian;  // 由于 y 轴翻转，旋转角度需要变负
    IMAGE* pImg = img->GetSrc();
    float fSin = (float)sin(radian), fCos = (float)cos(radian);
    float fNSin = (float)sin(-radian), fNCos = (float)cos(-radian);

    int w = pImg->getwidth(), h = pImg->getheight();
    DWORD* pBuf = GetImageBuffer(pImg);

    int left = 0, top = 0, right = 0, bottom = 0;        // 旋转后图像顶点
    POINT points[4] = {{0, 0}, {w, 0}, {0, h}, {w, h}};  // 存储图像顶点
    for (int j = 0; j < 4; j++)                          // 旋转图像顶点，搜索旋转后的图像边界
    {
        points[j] = {(int)(points[j].x * fCos - points[j].y * fSin),
                     (int)(points[j].x * fSin + points[j].y * fCos)};
        if (points[j].x < points[left].x) left = j;
        if (points[j].y > points[top].y) top = j;
        if (points[j].x > points[right].x) right = j;
        if (points[j].y < points[bottom].y) bottom = j;
    }

    int nw = points[right].x - points[left].x;  // 旋转后的图像尺寸
    int nh = points[top].y - points[bottom].y;
    int nSize = nw * nh;
    int offset_x = points[left].x < 0 ? points[left].x
                                      : 0;  // 旋转后图像超出第一象限的位移（据此调整图像位置）
    int offset_y = points[bottom].y < 0 ? points[bottom].y : 0;

    TImage* result = new TImage();
    IMAGE* rImg = result->GetSrc();
    Resize(rImg, nw, nh);
    DWORD* pNewBuf = GetImageBuffer(rImg);

    for (int i = offset_x, ni = 0; ni < nw;
         i++, ni++)  // i 用于映射原图像坐标，ni 用于定位旋转后图像坐标
    {
        for (int j = offset_y, nj = 0; nj < nh; j++, nj++) {
            int nx = (int)(i * fNCos - j * fNSin);  // 从旋转后的图像坐标向原图像坐标映射
            int ny = (int)(i * fNSin + j * fNCos);
            if (nx >= 0 && nx < w && ny >= 0 && ny < h)  // 若目标映射在原图像范围内，则拷贝色值
                pNewBuf[nj * nw + ni] = pBuf[ny * w + nx];
        }
    }

    // 信息存储
    result->m_width = rImg->getwidth();
    result->m_height = rImg->getheight();
    result->m_spriteN = 1;
    result->m_spriteRegion = {0.0f, 0.0f, result->m_width, result->m_height};

    gameaf::log("({}, {}) -> ({}, {})", img->GetWidth(), img->GetHeight(), result->GetWidth(),
                result->GetHeight());
    return result;
#else
    // ......
    return nullptr;
#endif
}

/// @brief 加载字体
/// @param path 字体路径
/// @note 注意后续字体的名字就是后缀.ttf前面的name
inline bool LoadFont(const std::wstring& path)
{
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    if (AddFontResourceEx(path.c_str(), FR_PRIVATE, nullptr)) {
        setbkmode(TRANSPARENT);
        return true;
    }
    return false;
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