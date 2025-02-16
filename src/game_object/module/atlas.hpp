#pragma once

#include <string>
#include <vector>

#include "image.hpp"

/**
 * 图集类
 * 由多个image组成(EasyX中)
 */

namespace gameaf {
class Atlas
{
public:
    /// @brief 加载图集资源
    /// @param atlas_path 图集路径, 类似`Assets/xxx/xxx_%d.png(jpg)`
    /// @param num 图集的图片个数(%d从1开始)
    bool load(const std::string& atlas_path, int num)
    {
        images.clear();
        images.reserve(num);

        char img_path[256];
        for (int i = 1; i <= num; ++i) {
            Image img;
            sprintf_s(img_path, atlas_path.c_str(), i);
            if (!img.load(img_path)) return false;
            images.emplace_back(img);
        }

        return true;
    }

    void clear() { images.clear(); }

    size_t size() const { return images.size(); }

    Image* getImg(int index)
    {
        if (index < 0 || index >= images.size()) return nullptr;
        return &images.at(index);
    }

    /// @note 注意返回出的对象是new出来的, 防止内存泄露
    Atlas* fliped()
    {
        Atlas* fliped_atlas = new Atlas();
        fliped_atlas->images = images;
        fliped_atlas->flip();
        return fliped_atlas;
    }

    void flip()
    {
        for (auto& img : images) {
            img.flip();
        }
    }

private:
    std::vector<Image> images;
};
}  // namespace gameaf