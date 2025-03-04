#pragma once

#include <common/Common.h>
#include <scene/Camera.h>

#include <common/Vector2.hpp>
#include <string>

namespace gameaf {

class TImage;
class Image
{
public:
    Image();
    Image(const Image&);
    Image(Image&&);
    ~Image();

public:
    /// @brief 内部加载图元
    /// @param imgPath 图元路径
    /// @param rows 图像在水平方向上均分个数
    /// @param cols 图像在竖直方向上均分个数
    /// @param spriteN 图像精灵图个数
    /// @return 加载是否成功
    /// @note rows+cols 配合精灵图个数可以实现均分图像上的任意个
    bool Load(const std::string& imgPath, int rows = 1, int cols = 1, size_t spriteN = 1);
    // 内部渲染使用
    void OnRender(const Camera& camera, const Rect& rect);

public:
    /// @brief 水平方向翻转自身
    void Flip();

    /// @brief 获取图像大小
    const Vector2& GetSize() const;

    /// @brief 设置图像大小
    void SetSize(Vector2 size);

    /// @brief 获取图元的宽
    int GetWidth() const;

    /// @brief 获取图元的高
    int GetHeight() const;

    /// @brief 返回精灵图的总数
    int GetSpriteNum() const;

    /// @brief 返回均分精灵图的矩形
    /// @param index 第几个(从左到右, 从上到下, 从零开始)
    /// @return 对应精灵图的矩形区域
    /// @note 如果index访问错误,则返回个空矩形区域
    Rect GetSpriteRect(size_t index) const;

private:
    TImage* m_img;        // 图像元
    Vector2 m_size;       // 图像大小
    size_t m_spriteN;     // 图像精灵图个数
    Rect m_spriteRegion;  // 描述每个精灵图的区域
    int m_width;          // 图元的宽
    int m_height;         // 图元的高
};

}  // namespace gameaf
