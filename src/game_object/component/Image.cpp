#include "Image.h"

#include <game_object/GameObject.h>

#include <common/MediaUtils.hpp>
#include <game_object/component/TImage.hpp>

namespace gameaf {

Image::Image(GameObject* obj, TImage* timg, Vector2 offset)
    : Component{obj, offset},
      m_img(timg),
      m_size(timg->GetSpriteRect(0).w, timg->GetSpriteRect(0).h),
      m_currentImg(timg)
{
}

Image::~Image() {}

/// @brief 辅助计算渲染目标矩形
/// @param pos 世界坐标
/// @param size 图像组件大小
/// @param anchor_pos 在单位矩形中描述锚点位置
/// @return
inline static Rect BuildRender(const Vector2& pos, const Vector2& size, const Vector2& anchor_pos)
{
    Rect dst;
    dst.w = static_cast<int>(size.X);
    dst.h = static_cast<int>(size.Y);
    dst.x = pos.X - size.X * anchor_pos.X;
    dst.y = pos.Y - size.Y * anchor_pos.Y;
    return dst;
}

void Image::OnRender(const Camera& camera) const { OnRender(camera, m_renderSpriteIndex); }

void Image::OnRender(const Camera& camera, size_t spriteIndex) const
{
    PutImageEx(camera, m_currentImg, BuildRender(Position(), m_size, m_anchorPosition),
               m_currentImg->GetSpriteRect(spriteIndex));
}

void Image::Flip()
{
    if (m_currentImg == m_img) {
        if (m_flipImg == nullptr) {
            m_flipImg = std::make_shared<TImage>(*m_img);
            m_flipImg->Flip();
        }
        m_currentImg = m_flipImg.get();
    } else {
        m_currentImg = m_img;
    }
}

const Vector2& Image::GetSize() const { return m_size; }

void Image::SetSizeScale(const Vector2& scale)
{
    m_size = Vector2{m_img->GetSpriteRect(0).w * 1.0f, m_img->GetSpriteRect(0).h * 1.0f} * scale;
    m_scale = scale;
}

size_t Image::GetSpriteNum() const { return m_img->GetSpriteNum(); }

Rect Image::GetSpriteRect(size_t index) const { return m_img->GetSpriteRect(index); }

void Image::SetAnchorMode(ImageAnchorMode mod, const Vector2& anchor_position)
{
    m_anchorMode = mod;
    switch (m_anchorMode) {
        case ImageAnchorMode::Centered:
            m_anchorPosition = Vector2{0.5f, 0.5f};
            break;
        case ImageAnchorMode::BottomCentered:
            m_anchorPosition = Vector2{0.5f, 1.0f};
            break;
        case ImageAnchorMode::TopCentered:
            m_anchorPosition = Vector2{0.5f, 0.0f};
            break;
        case ImageAnchorMode::Customized:
            m_anchorPosition = Vector2{std::clamp(anchor_position.X, 0.0f, 1.0f),
                                       std::clamp(anchor_position.Y, 0.0f, 1.0f)};
            break;
        default:
            break;
    }
}

bool Image::ContainsScreenPoint(const Camera& camera, const Vector2& pos) const
{
    Rect dst = BuildRender(Position(), m_size, m_anchorPosition);
    dst.x -= camera.GetPosition().X;
    dst.y -= camera.GetPosition().Y;
    return (pos.X >= dst.x && pos.X <= dst.x + dst.w) && (pos.Y >= dst.y && pos.Y <= dst.y + dst.h);
}

void Image::SetRenderSpriteIndex(size_t renderSpriteIndex)
{
    m_renderSpriteIndex = renderSpriteIndex;
}

// 测试旋转 debug 存在问题, 先不使用旋转
void Image::Rotate(double radian)
{
    // 存在内存泄露(替换后的m_img 为new出来对象, img组件删除时, 不会携带释放其内存)
    // m_currentImg = RotateImageEx(m_img, radian);
    // m_size = {m_currentImg->GetWidth() * 1.0f, m_currentImg->GetHeight() * 1.0f};
    // m_size *= m_scale;
}

}  // namespace gameaf