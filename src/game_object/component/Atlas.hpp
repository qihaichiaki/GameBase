#pragma once

#include <game_object/component/TImage.hpp>
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
    /// @param num 图集的图片个数(%d从0开始)
    bool Load(const std::string& atlas_path, int num)
    {
        timages.clear();
        timages.reserve(num);

        char img_path[256];
        for (int i = 0; i < num; ++i) {
            TImage timg;
            sprintf_s(img_path, atlas_path.c_str(), i);
            if (!timg.Load(img_path)) return false;
            timages.emplace_back(timg);
        }

        return true;
    }

    void Clear() { timages.clear(); }

    size_t Size() const { return timages.size(); }

    TImage* GetTImg(int index)
    {
        if (index < 0 || index >= timages.size()) return nullptr;
        return &timages.at(index);
    }

    void Flip()
    {
        for (auto& timg : timages) {
            timg.Flip();
        }
    }

private:
    std::vector<TImage> timages;
};
}  // namespace gameaf