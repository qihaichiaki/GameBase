#include "Image.h"

#include <common/MediaUtils.hpp>
#include <game_object/component/TImage.hpp>

namespace gameaf {

Image::Image() { m_img = new TImage{}; }
Image::Image(const Image& img)
{
    m_img = new TImage(*img.m_img);
    m_size = img.m_size;
    m_spriteN = img.m_spriteN;
    m_spriteRegion = img.m_spriteRegion;
    m_width = img.m_width;
    m_height = img.m_height;
}
Image::Image(Image&& img)
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

void Image::OnRender(const Camera& camera, const Rect& dst) { PutImageEx(camera, *m_img, dst); }

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

}  // namespace gameaf