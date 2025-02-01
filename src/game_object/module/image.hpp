#pragma once

#include "../common/macros.h"

namespace gameaf {
class Image
{
public:
    Image() = default;
    ~Image() = default;

public:
    /// @brief 返回图片的宽
    int getWidth() const
    {
#ifdef GAMEAF_USE_EASYX
        return m_image.getwidth();
#else
#endif
    }

    /// @brief 返回图片的高
    int getHeight() const
    {
#ifdef GAMEAF_USE_EASYX
        return m_image.getheight();
#else
#endif
    }

#ifdef GAMEAF_USE_EASYX
    /// @brief 获取适配easyx库的image对象
    IMAGE* getSrc() { return &m_image; }
#else
// todo ......
#endif

private:
#ifdef GAMEAF_USE_EASYX
    IMAGE m_image;
#else
// todo ......
#endif
};
}  // namespace gameaf