#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace gameaf {
class TImage;
class Atlas;

class ResourceManager
{
public:
    using TImagePtr = std::unique_ptr<TImage>;
    using AtlasPtr = std::unique_ptr<Atlas>;

public:
    static ResourceManager& GetInstance();

    /// @brief 加载图像资源
    /// @param image_path 图像路径
    /// @param image_name 图像名称
    /// @param rows 切分精灵图的行数
    /// @param cols 切分精灵图的列数
    /// @param spriteN 实际精灵图总个数
    /// @param isFlip 是否翻转后加载
    /// @return 是否创建成功
    bool NewImage(const std::string& image_path, const std::string& image_name, int rows = 1,
                  int cols = 1, size_t spriteN = 0, bool isFlip = false);
    /// @brief 加载图集资源
    /// @param atlas_path "xxx%dxxx.png/jpg", 注意下标从0开始
    /// @param n_images 几张图集
    /// @param atlas_name 加载后图集名称
    /// @param isFlip 是否翻转后加载
    bool LoadAtlas(const std::string& atlas_path, size_t n_images, const std::string& atlas_name,
                   bool isFlip = false);

    /// @brief 加载字体资源
    bool LoadFont(const std::string& font_path);

    /// @brief 加载音频资源
    bool LoadAudio(const std::string& audio_path, const std::string& audio_name);

    /// @brief 卸载图像资源
    void UnloadImage(const std::string& image_name);
    /// @brief 卸载字体资源
    void UnloadFont(const std::string& font_path);
    /// @brief 卸载图集资源
    void UnloadAtlas(const std::string& atlas_name);
    /// @brief 卸载音频资源
    void UnloadAudio(const std::string& audio_name);

    // 获取图像资源
    TImage* GetTImage(const std::string& image_path) const;

    // 获取图集资源
    Atlas* GetAtlas(const std::string& atlas_path) const;

private:
    ResourceManager();
    ~ResourceManager();
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

private:
    std::unordered_map<std::string, TImagePtr> m_images;
    std::unordered_map<std::string, AtlasPtr> m_atlases;
};
}  // namespace gameaf