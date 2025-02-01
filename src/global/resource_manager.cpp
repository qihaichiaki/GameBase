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

bool ResourceManager::loadImage(const std::string& image_path, const std::string& image_name)
{
    Image* load_img = gameaf::loadImage(image_path);
    if (load_img == nullptr) {
        gameaf::log("[error]位置: {}, 图像加载失败......", image_path);
        return false;
    }

    if (m_images.count(image_name) != 0) {
        gameaf::log("[error]图像名字: {}, 图像id重名......", image_name);
        return false;
    }
    m_images.emplace(image_name, load_img);
    return true;
}
bool ResourceManager::loadFont(const std::string& font_path) { return false; }
bool ResourceManager::loadAtlas(const std::string& atlas_path, size_t n_images,
                                const std::string& atlas_name)
{
    return false;
}
bool ResourceManager::loadAudio(const std::string& audio_path, const std::string& audio_name)
{
    return false;
}

void ResourceManager::unloadImage(const std::string& image_name) {}
void ResourceManager::unloadFont() {}
void ResourceManager::unloadAtlas(const std::string& atlas_name) {}
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