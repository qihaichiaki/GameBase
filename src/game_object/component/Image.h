#pragma once

#include <common/Common.h>
#include <game_object/component/Component.h>
#include <scene/Camera.h>

#include <common/Vector2.hpp>
#include <string>

namespace gameaf {

enum class ImageAnchorMode { Centered, BottomCentered, TopCentered, Customized };

class TImage;
class Image : public Component
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

    /// @brief 渲染图像组件
    /// @param camera 相机
    /// @param spriteIndex 精灵图id
    void OnRender(const Camera& camera, size_t spriteIndex = 0);

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

    /// @brief 设置锚点模式
    /// @param mod 锚点模式
    /// @param anchor_position 如果自定义,
    /// 请自由的设置锚点在x方向所占的比例和在y方向所占的比例(单位矩形).值的取值在[0,1]
    void SetAnchorMode(ImageAnchorMode mod, const Vector2& anchor_position = {0.0f, 0.0f});

private:
    TImage* m_img;        // 图像元
    Vector2 m_size;       // 图像大小
    size_t m_spriteN;     // 图像精灵图个数
    Rect m_spriteRegion;  // 描述每个精灵图的区域
    int m_width;          // 图元的宽
    int m_height;         // 图元的高

    // 锚点
    ImageAnchorMode m_anchorMode = ImageAnchorMode::Centered;  // 默认锚点位置在图片中心
    Vector2 m_anchorPosition = {0.5f, 0.5f};  // 锚点单位矩形内的坐标, 默认为图像中心
};

}  // namespace gameaf
