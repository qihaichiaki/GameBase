#include "ResourceManager.h"

#include <game_object/component/AudioManager.h>
#include <game_object/component/Image.h>

#include <common/Log.hpp>
#include <common/MediaUtils.hpp>
#include <game_object/component/Atlas.hpp>

namespace gameaf {

ResourceManager::ResourceManager() {}
ResourceManager::~ResourceManager() {}

ResourceManager& ResourceManager::GetInstance()
{
    static ResourceManager instance;
    return instance;
}

bool ResourceManager::NewImage(const std::string& image_path, const std::string& image_name,
                               int rows, int cols, size_t spriteN)
{
    if (m_images.count(image_name) != 0) {
        gameaf::log("[error][LoadImage] 图像名字:{}, 图像id重名惹......", image_name);
        return false;
    }

    Image* load_img = new Image();
    if (!load_img->Load(image_path, rows, cols, spriteN)) {
        gameaf::log("[error][loadImage] 位置: \"{}\", 图像加载失败惹......", image_path);
        delete load_img;
        return false;
    }

    m_images.emplace(image_name, load_img);
    return true;
}
bool ResourceManager::LoadFont(const std::string& font_path)
{
    gameaf::LoadFont(font_path);
    return true;
}
bool ResourceManager::LoadAtlas(const std::string& atlas_path, size_t n_images,
                                const std::string& atlas_name)
{
    if (m_atlases.count(atlas_name) != 0) {
        gameaf::log("[error][LoadAtlas] 图集名字: {}, 图集id重名惹......", atlas_name);
        return false;
    }

    Atlas* load_atlas = new Atlas();
    if (!load_atlas->Load(atlas_path, n_images)) {
        gameaf::log("[error][LoadAtlas] 位置: \"{}\", 图集加载失败惹......", atlas_path);
        delete load_atlas;
        return false;
    }

    m_atlases.emplace(atlas_name, load_atlas);
    return true;
}
bool ResourceManager::LoadAudio(const std::string& audio_path, const std::string& audio_name)
{
    AudioManager::GetInstance().OpenAudio(audio_path, audio_name);
    return false;
}

bool ResourceManager::FlipImage(const std::string& image_name, const std::string& flip_image_name)
{
    if (m_images.count(image_name) == 0 || m_images.count(flip_image_name) != 0) return false;

    auto flipImage = std::make_unique<Image>(*m_images.at(image_name));  // 拷贝构造
    flipImage->Flip();
    m_images.emplace(flip_image_name, std::move(flipImage));
    return true;
}
bool ResourceManager::FlipAtlas(const std::string& atlas_name, const std::string& flip_atlas_name)
{
    if (m_atlases.count(atlas_name) == 0 || m_atlases.count(flip_atlas_name) != 0) return false;
    auto flipAtlas = std::make_unique<Atlas>(*m_atlases.at(atlas_name));  // 拷贝构造
    flipAtlas->Flip();
    m_atlases.emplace(flip_atlas_name, std::move(flipAtlas));
    return true;
}

void ResourceManager::UnloadImage(const std::string& image_name) { m_images.erase(image_name); }

void ResourceManager::UnloadFont(const std::string& font_path) { gameaf::UnLoadFont(font_path); }

void ResourceManager::UnloadAtlas(const std::string& atlas_name) { m_atlases.erase(atlas_name); }

void ResourceManager::UnloadAudio(const std::string& audio_name)
{
    AudioManager::GetInstance().CloseAudio(audio_name);
}

Image* ResourceManager::GetImage(const std::string& image_name) const
{
    if (m_images.count(image_name) == 0) return nullptr;
    return m_images.at(image_name).get();
}
Atlas* ResourceManager::GetAtlas(const std::string& atlas_name) const
{
    if (m_atlases.count(atlas_name) == 0) return nullptr;
    return m_atlases.at(atlas_name).get();
}
}  // namespace gameaf