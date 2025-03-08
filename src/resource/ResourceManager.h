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
    bool NewImage(const std::string& image_path, const std::string& image_name, int rows = 1,
                  int cols = 1, size_t spriteN = 1);
    /// @brief 加载字体资源
    bool LoadFont(const std::string& font_path);
    /// @brief 加载图集资源
    bool LoadAtlas(const std::string& atlas_path, size_t n_images, const std::string& atlas_name);
    /// @brief 加载音频资源
    bool LoadAudio(const std::string& audio_path, const std::string& audio_name);

    /// @brie 翻转图像, 产生新的图像
    bool FlipImage(const std::string& image_name, const std::string& flip_image_name);
    /// @brief 翻转图集, 产生新的图集
    bool FlipAtlas(const std::string& atlas_name, const std::string& flip_atlas_name);

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