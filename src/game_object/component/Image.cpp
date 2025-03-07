#include "Image.h"

#include <game_object/GameObject.h>

#include <common/MediaUtils.hpp>
#include <game_object/component/TImage.hpp>

namespace gameaf {

Image::Image() : Component(nullptr, Vector2{}) { m_img = new TImage{}; }
Image::Image(const Image& img) : Component(img.m_gameObject, img.m_offset)
{
    m_img = new TImage(*img.m_img);
    m_size = img.m_size;
    m_spriteN = img.m_spriteN;
    m_spriteRegion = img.m_spriteRegion;
    m_width = img.m_width;
    m_height = img.m_height;
}
Image::Image(Image&& img) : Component(img.m_gameObject, img.m_offset)
{
    m_img = img.m_img;
    img.m_img = nullptr;
    m_size = img.m_size;
    m_spriteN = img.m_spriteN;
    m_spriteRegion = img.m_spriteRegion;
    m_width = img.m_width;
    m_height = img.m_height;
}
Image::~Image() { delete m_img; }

bool Image::Load(const std::string& imgPath, int rows, int cols, size_t spriteN)
{
    if (!m_img->Load(imgPath)) return false;
    m_spriteN = spriteN;
    m_width = m_img->GetWidth();
    m_height = m_img->GetHeight();
    m_size = {m_width * 1.0f, m_height * 1.0f};
    m_spriteRegion = {0.0f, 0.0f, m_width / rows, m_height / cols};
    return true;
}

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
    dst.x = static_cast<int>(pos.X - size.X * anchor_pos.X);
    dst.y = static_cast<int>(pos.Y - size.Y * anchor_pos.Y);
    return dst;
}

void Image::OnRender(const Camera& camera, size_t spriteIndex)
{
    PutImageEx(camera, *m_img, BuildRender(m_gameObject->GetPosition(), m_size, m_anchorPosition),
               GetSpriteRect(spriteIndex));
}

void Image::Flip() { m_img->Flip(m_spriteN, m_spriteRegion.w, m_spriteRegion.h); }

const Vector2& Image::GetSize() const { return m_size; }

void Image::SetSize(Vector2 size) { m_size = size; }

int Image::GetWidth() const { return m_width; }

int Image::GetHeight() const { return m_height; }

int Image::GetSpriteNum() const { return m_spriteN; }

Rect Image::GetSpriteRect(size_t index) const
{
    if (index > m_spriteN) return {};
    int x = (index * m_spriteRegion.w) % m_width;
    int y = ((index * m_spriteRegion.w) / m_width) * m_spriteRegion.h;
    return {x * 1.0f, y * 1.0f, m_spriteRegion.w, m_spriteRegion.h};
}

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