#include <component_tool.h>
#include <game_object.h>
#include <resource_manager.h>

#include "animator.h"
#include "collision_manager.hpp"
#include "rigidbody_manager.hpp"

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
        gameaf::log("[warring][ImageTool::create]{} \"{}\" image组件加载失败", game_obj.getName(),
                    img_id);
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

// ======CollisionTool======
bool CollisionTool::setCollisionEnabled(GameObject& game_obj, bool enabled)
{
    if (game_obj.m_collision_component == nullptr) return false;
    game_obj.m_collision_component->setEnabled(enabled);
    return true;
}

bool CollisionTool::setOnCollide(GameObject& game_obj, std::function<void(GameObject&)> on_collide)
{
    if (game_obj.m_collision_component == nullptr) return false;
    game_obj.m_collision_component->setOnCollide(on_collide);
    return true;
}

bool CollisionTool::setSrcLayer(GameObject& game_obj, Collisionlayer src_layer)
{
    if (game_obj.m_collision_component == nullptr) return false;
    game_obj.m_collision_component->setSrcLayer(src_layer);
    return true;
}

bool CollisionTool::addDstLayer(GameObject& game_obj, Collisionlayer dst_layer)
{
    if (game_obj.m_collision_component == nullptr) return false;
    game_obj.m_collision_component->addDstLayer(dst_layer);
    return true;
}

bool CollisionTool::createCollisionBox(GameObject& game_obj)
{
    CollisionBox* collision_box = nullptr;
    if (game_obj.m_collision_component &&
        game_obj.m_collision_component->type() != CollisionComponentType::Box) {
        gameaf::log("[warring][CollisionTool::createCollisionBox]{} 已经存在不为box类型的碰撞器",
                    game_obj.getName());
        return false;
    }
    if (game_obj.m_collision_component == nullptr) {
        collision_box = CollisionManager::getInstance().createCollisionBox(&game_obj);
        game_obj.m_collision_component = collision_box;
    } else {
        collision_box = static_cast<CollisionBox*>(game_obj.m_collision_component);
    }

    if (game_obj.m_image) {
        collision_box->size() = game_obj.m_img_size;
    } else if (game_obj.m_animator) {
        collision_box->size() = game_obj.m_animator->initialAnimation().currentFrameSize();
    }
    return true;
}

bool CollisionTool::modCollisionBox(GameObject& game_obj, float factor_x, float factor_y)
{
    if (!createCollisionBox(game_obj)) return false;

    auto collision_box = static_cast<CollisionBox*>(game_obj.m_collision_component);
    collision_box->size() *= Vector2{factor_x, factor_y};
    return true;
}

bool CollisionTool::modCollisionBox(GameObject& game_obj, Vector2 delta)
{
    if (!createCollisionBox(game_obj)) return false;
    auto collision_box = game_obj.m_collision_component;
    collision_box->setOffset(delta);
    return true;
}

bool CollisionTool::setCollisionBoxSize(GameObject& game_obj, Vector2 size)
{
    if (!createCollisionBox(game_obj)) return false;

    auto collision_box = static_cast<CollisionBox*>(game_obj.m_collision_component);
    collision_box->size() = size;
    return true;
}

Vector2 CollisionTool::getCollisionBoxSize(GameObject& game_obj)
{
    if (!createCollisionBox(game_obj)) return {};

    auto collision_box = static_cast<CollisionBox*>(game_obj.m_collision_component);
    return collision_box->size();
}

Vector2 CollisionTool::getCollisionPostion(GameObject& game_obj)
{
    if (!createCollisionBox(game_obj)) return {};

    return game_obj.m_collision_component->getPosition();
}

// ====== RigidbodyTool ======
bool RigidbodyTool::createRigidbody2D(GameObject& game_obj)
{
    if (game_obj.m_rigidbody2D != nullptr) {
        gameaf::log("[warring][RigidbodyTool::createRigidbody2D]{} 已经存在2d刚体组件",
                    game_obj.getName());
        return false;
    }
    game_obj.m_rigidbody2D = RigidbodyManager::getInstance().CreateRigidbody2D(&game_obj);
    return true;
}

Vector2 RigidbodyTool::velocity(GameObject& game_obj)
{
    if (game_obj.m_rigidbody2D == nullptr) {
        gameaf::log("[error][RigidbodyTool::velocity]{} 不存在2d刚体组件", game_obj.getName());
        return {};
    }
    return game_obj.m_rigidbody2D->velocity();
}

void RigidbodyTool::setVelocity(GameObject& game_obj, const Vector2& velocity)
{
    if (game_obj.m_rigidbody2D == nullptr) {
        gameaf::log("[error][RigidbodyTool::velocity]{} 不存在2d刚体组件", game_obj.getName());
        return;
    }
    game_obj.m_rigidbody2D->velocity() = velocity;
}

void RigidbodyTool::setGravityScale(GameObject& game_obj, float gravity_scale)
{
    if (game_obj.m_rigidbody2D == nullptr) {
        gameaf::log("[error][RigidbodyTool::velocity]{} 不存在2d刚体组件", game_obj.getName());
        return;
    }
    game_obj.m_rigidbody2D->gravityScale() = gravity_scale;
}

float RigidbodyTool::gravityScale(GameObject& game_obj)
{
    if (game_obj.m_rigidbody2D == nullptr) {
        gameaf::log("[error][RigidbodyTool::velocity]{} 不存在2d刚体组件", game_obj.getName());
        return 0.0f;
    }
    return game_obj.m_rigidbody2D->gravityScale();
}

}  // namespace gameaf