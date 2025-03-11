#include "Image.h"

#include <game_object/GameObject.h>

#include <common/MediaUtils.hpp>
#include <game_object/component/TImage.hpp>

namespace gameaf {

Image::Image(GameObject* obj, TImage* timg, Vector2 offset)
    : Component{obj, offset},
      m_img(timg),
      m_size(timg->GetWidth(), timg->GetHeight()),
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

void Image::OnRender(const Camera& camera, size_t spriteIndex) const
{
    PutImageEx(camera, m_currentImg,
               BuildRender(m_gameObject->GetPosition() + m_offset, m_size, m_anchorPosition),
               m_img->GetSpriteRect(spriteIndex));
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

void Image::SetSize(Vector2 size) { m_size = size; }

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

}  // namespace gameaf