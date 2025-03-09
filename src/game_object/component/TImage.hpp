#pragma once

#include <common/Common.h>
#include <common/Macros.h>

#include <cmath>
#include <common/Utils.hpp>
#include <string>

namespace gameaf {

class TImage
{
public:
    TImage() = default;
    ~TImage() = default;

public:
    /// @brief 加载图像
    /// @param imgPath 图像路径
    /// @param rows 图像在水平方向上均分个数
    /// @param cols 图像在竖直方向上均分个数
    /// @param spriteN 图像精灵图个数
    /// @return 加载是否成功
    /// @note rows+cols 配合精灵图个数可以实现均分图像上的任意个
    bool Load(const std::string& img_path, int rows = 1, int cols = 1, size_t spriteN = 1)
    {
#ifdef GAMEAF_USE_EASYX
        loadimage(&m_image, UTF8StrToWStr(img_path).c_str());
        m_width = m_image.getwidth();
        m_height = m_image.getheight();
        if (m_width == 0 || m_height == 0) return false;
#else
#endif
        m_spriteN = spriteN;
        m_spriteRegion = {0.0f, 0.0f, m_width / rows, m_height / cols};
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

    /// @brief 返回精灵图的总数
    size_t GetSpriteNum() const { return m_spriteN; }

    /// @brief 返回均分精灵图的矩形
    /// @param index 第几个(从左到右, 从上到下, 从零开始)
    /// @return 对应精灵图的矩形区域
    /// @note 如果index访问错误,则返回个空矩形区域
    Rect GetSpriteRect(size_t index) const
    {
        if (index > m_spriteN) return {};
        int x = (index * m_spriteRegion.w) % m_width;
        int y = ((index * m_spriteRegion.w) / m_width) * m_spriteRegion.h;
        return {x * 1.0f, y * 1.0f, m_spriteRegion.w, m_spriteRegion.h};
    }

    /// @brief 翻转自身
    void Flip()
    {
        int x = 0, y = 0;
#ifdef GAMEAF_USE_EASYX
        IMAGE copy_img;
        Resize(&copy_img, m_width, m_height);
        copy_img = m_image;

        for (int i = 0; i < m_spriteN; ++i) {
            x += i * m_spriteRegion.w;
            if (x == m_width) {
                x = 0;
                y += m_spriteRegion.h;
                continue;
            }

            FlipImage(&copy_img, &m_image,
                      {x * 1.0f, y * 1.0f, m_spriteRegion.w, m_spriteRegion.h});
        }
#endif
    }

    /// @brief 产生一个基于自身水平翻转newTImage
    /// @warning 注意这是一个new出来的对象, 不使用了需要delete, 防止内存泄露
    TImage* Fliped() const
    {
        auto flipedTImage = new TImage(*this);
        flipedTImage->Flip();
        return flipedTImage;
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
    size_t m_spriteN;     // 图像精灵图个数
    Rect m_spriteRegion;  // 描述每个精灵图的区域

#ifdef GAMEAF_USE_EASYX
    IMAGE m_image;
#else
// todo ......
#endif
};

}  // namespace gameaf