#pragma once

#include <common/Macros.h>

#include <cmath>
#include <string>

namespace gameaf {

class TImage
{
public:
    TImage() = default;
    ~TImage() = default;

public:
    /// @brief 加载图像
    /// @param img_path 图像路径
    bool Load(const std::string& img_path)
    {
#ifdef GAMEAF_USE_EASYX
        loadimage(&m_image, _T(img_path.c_str()));
        m_width = m_image.getwidth();
        m_height = m_image.getheight();
        if (m_width == 0 || m_height == 0) return false;
#else
#endif
        return true;
    }

#ifdef GAMEAF_USE_EASYX

    /// @brief 获取适配easyx库的image对象
    IMAGE* GetSrc() { return &m_image; }
#else
// todo ......
#endif

    /// @brief 返回图片的宽
    int GetWidth() const { return m_width; }

    /// @brief 返回图片的高
    int GetHeight() const { return m_height; }

    //     /// @brief 当前图片反转生成新的图片
    //     /// @note 注意image为new出来的对象, 防止内存泄露
    //     Image* fliped(size_t spriteN, int spriteW, int spriteH)
    //     {
    //         Image* result = new Image();
    //         int img_width = getWidth();
    //         int img_height = getHeight();
    // #ifdef GAMEAF_USE_EASYX
    //         // Resize(result->getSrc(), img_width, img_height);
    //         *result = *this;
    // #else
    // #endif
    //         result->flip();
    //         return result;
    //     }

    /// @brief 翻转自身
    /// @param spriteN 精灵图个数
    /// @param spriteW 精灵图宽度
    /// @param spriteH 精灵图高度
    void Flip(size_t spriteN, int spriteW, int spriteH)
    {
        int x = 0, y = 0;
#ifdef GAMEAF_USE_EASYX
        IMAGE copy_img;
        Resize(&copy_img, m_width, m_height);
        copy_img = m_image;

        for (int i = 0; i < spriteN; ++i) {
            x += i * spriteW;
            if (x == m_width) {
                x = 0;
                y += spriteH;
                continue;
            }

            FlipImage(&copy_img, &m_image, {x * 1.0f, y * 1.0f, spriteW, spriteH});
        }
#endif
    }

private:
#ifdef GAMEAF_USE_EASYX
    /// @brief 在源图像的翻转矩形中进行翻转
    /// @param src_img 源图像
    /// @param dst_img 目标图像(注意没有Resize)
    /// @param flip_rect 翻转矩形
    void FlipImage(IMAGE* src_img, IMAGE* dst_img, const Rect& flip_rect)
    {
        int flip_x = flip_rect.x;
        int flip_y = flip_rect.y;
        int src_w = src_img->getwidth();
        DWORD* src = GetImageBuffer(src_img);
        DWORD* dst = GetImageBuffer(dst_img);

        // 画图理解一下
        for (int y = flip_y; y < flip_y + flip_rect.h; ++y) {
            for (int x = flip_x; x < flip_x + flip_rect.w; ++x) {
                dst[y * src_w + x] = src[y * src_w + (2 * flip_x - x + flip_rect.w - 1)];
            }
        }
    }
#else
#endif

private:
    int m_width = 0;
    int m_height = 0;

#ifdef GAMEAF_USE_EASYX
    IMAGE m_image;
#else
// todo ......
#endif
};

}  // namespace gameaf