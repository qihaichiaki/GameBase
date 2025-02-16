#include "../include/resource_manager.h"

#include "../common/media_utils.hpp"
#include "../game_object/module/atlas.hpp"
#include "../game_object/module/image.hpp"
#include "../include/log.hpp"

namespace gameaf {

ResourceManager::ResourceManager() {}
ResourceManager::~ResourceManager() {}

ResourceManager& ResourceManager::getInstance()
{
    static ResourceManager instance;
    return instance;
}

bool ResourceManager::loadImage(const std::string& image_path, const std::string& image_name,
                                int rows, int cols)
{
    if (m_images.count(image_name) != 0) {
        gameaf::log("[error][loadImage] 图像名字: {}, 图像id重名惹......", image_name);
        return false;
    }

    Image* load_img = new Image();
    if (!load_img->load(image_path, rows, cols)) {
        gameaf::log("[error][loadImage] 位置: \"{}\", 图像加载失败惹......", image_path);
        delete load_img;
        return false;
    }

    m_images.emplace(image_name, load_img);
    return true;
}
bool ResourceManager::loadFont(const std::string& font_path) { return false; }
bool ResourceManager::loadAtlas(const std::string& atlas_path, size_t n_images,
                                const std::string& atlas_name)
{
    if (m_atlases.count(atlas_name) != 0) {
        gameaf::log("[error][loadAtlas] 图集名字: {}, 图集id重名惹......", atlas_name);
        return false;
    }

    Atlas* load_atlas = new Atlas();
    if (!load_atlas->load(atlas_path, n_images)) {
        gameaf::log("[error][loadAtlas] 位置: \"{}\", 图集加载失败惹......", atlas_path);
        delete load_atlas;
        return false;
    }

    m_atlases.emplace(atlas_name, load_atlas);
    return true;
}
bool ResourceManager::loadAudio(const std::string& audio_path, const std::string& audio_name)
{
    return false;
}

bool ResourceManager::flipImage(const std::string& image_name, const std::string& flip_image_name)
{
    if (m_images.count(image_name) == 0 || m_images.count(flip_image_name) != 0) return false;
    m_images.emplace(flip_image_name, m_images.at(image_name)->fliped());
    return true;
}
bool ResourceManager::flipAtlas(const std::string& atlas_name, const std::string& flip_atlas_name)
{
    if (m_atlases.count(atlas_name) == 0 || m_atlases.count(flip_atlas_name) != 0) return false;
    m_atlases.emplace(flip_atlas_name, m_atlases.at(atlas_name)->fliped());
    return true;
}

void ResourceManager::unloadImage(const std::string& image_name) { m_images.erase(image_name); }
void ResourceManager::unloadFont() {}
void ResourceManager::unloadAtlas(const std::string& atlas_name) { m_atlases.erase(atlas_name); }
void ResourceManager::unloadAudio(const std::string& audio_name) {}

Image* ResourceManager::getImage(const std::string& image_name) const
{
    if (m_images.count(image_name) == 0) return nullptr;
    return m_images.at(image_name).get();
}
Atlas* ResourceManager::getAtlas(const std::string& atlas_name) const
{
    if (m_atlases.count(atlas_name) == 0) return nullptr;
    return m_atlases.at(atlas_name).get();
}
}  // namespace gameaf