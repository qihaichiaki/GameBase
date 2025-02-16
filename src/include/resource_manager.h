#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace gameaf {
class Image;
class Atlas;

class ResourceManager
{
public:
    using ImagePtr = std::unique_ptr<Image>;
    using AtlasPtr = std::unique_ptr<Atlas>;

public:
    static ResourceManager& getInstance();

    /// @brief 加载图像资源
    bool loadImage(const std::string& image_path, const std::string& image_name, int rows = 1,
                   int cols = 1);
    /// @brief 加载字体资源
    bool loadFont(const std::string& font_path);
    /// @brief 加载图集资源
    bool loadAtlas(const std::string& atlas_path, size_t n_images, const std::string& atlas_name);
    /// @brief 加载音频资源
    bool loadAudio(const std::string& audio_path, const std::string& audio_name);

    /// @brie 翻转图像, 产生新的图像
    bool flipImage(const std::string& image_name, const std::string& flip_image_name);
    /// @brief 翻转图集, 产生新的图集
    bool flipAtlas(const std::string& atlas_name, const std::string& flip_atlas_name);

    /// @brief 卸载图像资源
    void unloadImage(const std::string& image_name);
    /// @brief 卸载字体资源
    void unloadFont();
    /// @brief 卸载图集资源
    void unloadAtlas(const std::string& atlas_name);
    /// @brief 卸载音频资源
    void unloadAudio(const std::string& audio_name);

public:
    /// @internal 内部函数使用

    // 获取资源
    Image* getImage(const std::string& image_path) const;
    Atlas* getAtlas(const std::string& atlas_path) const;

    /// @endinternal

private:
    ResourceManager();
    ~ResourceManager();
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

private:
    std::unordered_map<std::string, ImagePtr> m_images;
    std::unordered_map<std::string, AtlasPtr> m_atlases;
};
}  // namespace gameaf