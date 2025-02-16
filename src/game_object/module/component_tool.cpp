#include <component_tool.h>
#include <game_object.h>
#include <resource_manager.h>

#include "animator.h"

namespace gameaf {

// ======AnimatorTool======
bool AnimatorTool::newAnimationForImage(GameObject& game_obj, const std::string& animation_id,
                                        const std::string& img_id, float interval, bool is_loop)
{
    if (game_obj.m_animator == nullptr) {
        game_obj.m_animator = std::make_unique<Animator>();
    }

    return game_obj.m_animator->newAnimationForImage(animation_id, img_id, interval, is_loop);
}

bool AnimatorTool::newAnimationForAtlas(GameObject& game_obj, const std::string& animation_id,
                                        const std::string& atlas_id, float interval, bool is_loop)
{
    if (game_obj.m_animator == nullptr) {
        game_obj.m_animator = std::make_unique<Animator>();
    }

    return game_obj.m_animator->newAnimationForAtlas(animation_id, atlas_id, interval, is_loop);
}

bool AnimatorTool::delAnimation(const GameObject& game_obj, const std::string& animation_id)
{
    if (game_obj.m_animator == nullptr) return false;
    return game_obj.m_animator->delAnimation(animation_id);
}

bool AnimatorTool::setInitialAnimation(const GameObject& game_obj, const std::string& animation_id)
{
    if (game_obj.m_animator == nullptr) return false;
    return game_obj.m_animator->setInitialAnimation(animation_id);
}

bool AnimatorTool::switchToAnimation(const GameObject& game_obj, const std::string& animation_id)
{
    if (game_obj.m_animator == nullptr) return false;
    return game_obj.m_animator->switchToAnimation(animation_id);
}

// ======ImageTool======
bool ImageTool::create(GameObject& game_obj, const std::string& img_id)
{
    GameObject::ImagePtr new_img = ResourceManager::getInstance().getImage(img_id);
    if (new_img == nullptr) {
        gameaf::log("[warring][create] \"{}\" image组件加载失败", img_id);
        return false;
    }
    game_obj.m_image = new_img;
    game_obj.m_img_size = {new_img->getWidth() * 1.0f, new_img->getHeight() * 1.0f};
    return true;
}

void ImageTool::flip(GameObject& game_obj)
{
    if (game_obj.m_image != nullptr) game_obj.m_image->flip();
}

const Vector2& ImageTool::size(const GameObject& game_obj) { return game_obj.m_img_size; }

void ImageTool::setSize(GameObject& game_obj, Vector2 size) { game_obj.m_img_size = size; }

void ImageTool::scaleProportionally(GameObject& game_obj, float zoom_factor)
{
    game_obj.m_img_size *= zoom_factor;
}

void ImageTool::scale(GameObject& game_obj, const Vector2& zoom_factor)
{
    game_obj.m_img_size *= zoom_factor;
}

}  // namespace gameaf