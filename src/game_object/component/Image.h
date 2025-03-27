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
    Image(GameObject* obj, TImage* timg, Vector2 offset = Vector2{});
    ~Image();

public:
    /// @brief 渲染图像组件
    /// @param camera 相机
    void OnRender(const Camera& camera) const;

    /// @brief 渲染图像组件
    /// @param camera 相机
    /// @param spriteIndex 精灵图id
    void OnRender(const Camera& camera, size_t spriteIndex) const;

public:
    /// @brief 水平方向翻转自身
    void Flip() override;

    /// @brief 获取图像大小
    const Vector2& GetSize() const;

    /// @brief 设置图像大小比例
    void SetSizeScale(const Vector2& size);

    /// @brief 返回精灵图的总数
    size_t GetSpriteNum() const;

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

    /// @brief 检测屏幕坐标是否在图像组件内容
    bool ContainsScreenPoint(const Camera&, const Vector2&) const override;

    /// @brief 设置图像具体渲染哪张精灵图
    /// @param renderSpriteIndex 从0开头
    void SetRenderSpriteIndex(size_t renderSpriteIndex);

    /// @brief 旋转图像
    /// @param radian 逆时针旋转角度弧度值
    /// @note 默认根据锚点位置进行旋转
    void Rotate(double radian);

    /// @brief 加载新的图元
    void Load(TImage* timg);

private:
    // 由于使用的是easyx, 为了增加效率, 缓存量会非常大(确信)
    TImage* m_img;                      // 图像元
    Vector2 m_scale;                    // 图像元变化为当前图像大小的值, 用于计算最终渲染图像大小
    Vector2 m_size;                     // 当前的图像大小
    std::shared_ptr<TImage> m_flipImg;  // 辅助翻转使用的新图元, 生命周期共享于使用对应图元
    TImage* m_currentImg = nullptr;     // 辅助切换正常和翻转的图元

    // 锚点
    ImageAnchorMode m_anchorMode = ImageAnchorMode::Centered;  // 默认锚点位置在图片中心
    Vector2 m_anchorPosition = {0.5f, 0.5f};  // 锚点单位矩形内的坐标, 默认为图像中心

    size_t m_renderSpriteIndex = 0;  // 精灵图作为图像组件时, 需要渲染的第几张精灵图
};

}  // namespace gameaf
