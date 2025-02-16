#pragma once

#include <macros.h>

#include <cmath>

namespace gameaf {

class Image
{
public:
    struct Rect
    {
        int x, y;
        int w, h;
    };

public:
    Image() = default;
    ~Image() = default;

public:
    /// @brief 加载图像
    /// @param img_path 图像路径
    /// @param rows 均分为几列
    /// @param cols 均分为几行
    bool load(const std::string& img_path, int rows = 1, int cols = 1)
    {
#ifdef GAMEAF_USE_EASYX
        loadimage(&m_image, _T(img_path.c_str()));
        m_width = m_image.getwidth();
        m_height = m_image.getheight();
        if (m_width == 0 || m_height == 0) return false;
#else
#endif
        m_sprite_n = rows * cols;
        m_sprite_region = {0, 0, m_width / rows, m_height / cols};

        return true;
    }

#ifdef GAMEAF_USE_EASYX

    /// @brief 获取适配easyx库的image对象
    IMAGE* getSrc() { return &m_image; }
#else
// todo ......
#endif

    /// @brief 返回均分精灵图的矩形
    /// @param index 第几个(从左到右, 从上到下, 从零开始)
    Rect getSpriteRect(size_t index) const
    {
        if (index > m_sprite_n) return {};
        int x = (index * m_sprite_region.w) % getWidth();
        int y = ((index * m_sprite_region.w) / getWidth()) * m_sprite_region.h;
        return {x, y, m_sprite_region.w, m_sprite_region.h};
    }

    /// @brief 返回精灵图的总个数
    int getSpriteNum() const { return m_sprite_n; }

    /// @brief 返回图片的宽
    int getWidth() const { return m_width; }

    /// @brief 返回图片的高
    int getHeight() const { return m_height; }

    /// @brief 当前图片反转生成新的图片
    /// @note 注意image为new出来的对象, 防止内存泄露
    Image* fliped()
    {
        Image* result = new Image();
        int img_width = getWidth();
        int img_height = getHeight();
#ifdef GAMEAF_USE_EASYX
        // Resize(result->getSrc(), img_width, img_height);
        *result = *this;
#else
#endif
        result->flip();
        return result;
    }

    /// @brief 翻转自身
    void flip()
    {
        int x = 0, y = 0;
#ifdef GAMEAF_USE_EASYX
        IMAGE copy_img;
        Resize(&copy_img, m_width, m_height);
        copy_img = m_image;

        for (int i = 0; i < m_sprite_n; ++i) {
            x += i * m_sprite_region.w;
            if (x == m_width) {
                x = 0;
                y += m_sprite_region.h;
                continue;
            }

            flipImage(&copy_img, &m_image, {x, y, m_sprite_region.w, m_sprite_region.h});
        }

#endif
    }

private:
#ifdef GAMEAF_USE_EASYX
    /// @brief 在源图像的翻转矩形中进行翻转
    /// @param src_img 源图像
    /// @param dst_img 目标图像(注意没有Resize)
    /// @param flip_rect 翻转矩形
    void flipImage(IMAGE* src_img, IMAGE* dst_img, const Rect& flip_rect)
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
    Rect m_sprite_region;  // 精灵图的区域
    int m_sprite_n = 1;    // 当前图片有多少张精灵图

#ifdef GAMEAF_USE_EASYX
    IMAGE m_image;
#else
// todo ......
#endif
};
}  // namespace gameaf