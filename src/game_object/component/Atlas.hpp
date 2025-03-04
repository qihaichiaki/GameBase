#pragma once

#include <game_object/component/Image.h>

#include <string>
#include <vector>

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
    bool Load(const std::string& atlas_path, int num)
    {
        images.clear();
        images.reserve(num);

        char img_path[256];
        for (int i = 1; i <= num; ++i) {
            Image img;
            sprintf_s(img_path, atlas_path.c_str(), i);
            if (!img.Load(img_path)) return false;
            images.emplace_back(img);
        }

        return true;
    }

    void Clear() { images.clear(); }

    size_t Size() const { return images.size(); }

    Image* GetImg(int index)
    {
        if (index < 0 || index >= images.size()) return nullptr;
        return &images.at(index);
    }

    void Flip()
    {
        for (auto& img : images) {
            img.Flip();
        }
    }

private:
    std::vector<Image> images;
};
}  // namespace gameaf