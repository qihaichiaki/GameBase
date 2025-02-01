#pragma once

#include "../game_object/module/image.hpp"
#include "../include/log.hpp"
#include "../scene/camera.hpp"
#include "macros.h"

#ifdef _MSC_VER
#pragma comment(lib, "MSIMG32.lib")  // AlphaBlend
#else
#endif

/**
 * 媒体工具函数
 * 方便游戏框架渲染图形和设置音频相关
 */

namespace gameaf {

/// @brief 加载图像
/// @param img_path 图像的路径加文件名
/// @return 返回一个图像对象的指针, 注意内存是new过的, 注意不用了请delete释放内存防止内存泄露.
/// 如果图像加载无效, 返回nullptr
inline Image* loadImage(const std::string& img_path)
{
    Image* load_img = new Image{};
#ifdef GAMEAF_USE_EASYX
    loadimage(load_img->getSrc(), _T(img_path.c_str()));
    if (load_img->getHeight() == 0 || load_img->getWidth() == 0) {
        delete load_img;
        return nullptr;
    }
#else
// todo ......
#endif

    return load_img;
}

// 矩形, 通常由左上角位置(二维坐标系下) + 宽高组成
struct Rect
{
    int x, y;
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
    int dst_x = dst.x - static_cast<int>(camera.getPosition().X);
    int dst_y = dst.y - static_cast<int>(camera.getPosition().Y);
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
    putImageEx(camera, img, dst, {0, 0, img.getWidth(), img.getHeight()});
    gameaf::log("渲染[{}]图片目标位置[{}, {}] - 宽度:{}, 高度:{}", &img, dst.x, dst.y, dst.w,
                dst.h);
}

}  // namespace gameaf