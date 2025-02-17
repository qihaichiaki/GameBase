#pragma once

/**
 * gameobject component tool
 * 工具头文件
 */

#include <functional>
#include <string>

#include "collision_layer.hpp"
#include "vector2.hpp"

namespace gameaf {

class GameObject;

struct AnimatorTool
{
    /// @brief 通过Image给游戏对象添加动画
    /// @param game_obj 游戏对象
    /// @param animation_id 赋予的动画id
    /// @param img_id Image对象的id
    /// @param interval 动画帧切换的间隔时间
    /// @param is_loop 是否循环
    static bool newAnimationForImage(GameObject& game_obj, const std::string& animation_id,
                                     const std::string& img_id, float interval, bool is_loop);

    /// @brief 通过Atlas给游戏对象添加动画
    /// @param game_obj 游戏对象
    /// @param animation_id 赋予的动画id
    /// @param img_id Atlas对象的id
    /// @param interval 动画帧切换的间隔时间
    /// @param is_loop 是否循环
    static bool newAnimationForAtlas(GameObject& game_obj, const std::string& animation_id,
                                     const std::string& atlas_id, float interval, bool is_loop);

    /// @brief 删除游戏对象中的动画
    /// @param game_obj 游戏对象
    /// @param animation_id 动画id
    static bool delAnimation(const GameObject& game_obj, const std::string& animation_id);

    /// @brief 设置游戏对象初始的动画
    /// @param game_obj 游戏对象
    /// @param animation_id 动画id
    static bool setInitialAnimation(const GameObject& game_obj, const std::string& animation_id);

    /// @brief 切换游戏对象的动画
    /// @param game_obj 游戏对象
    /// @param animation_id 动画id
    static bool switchToAnimation(const GameObject& game_obj, const std::string& animation_id);
};

struct ImageTool
{
    /// @brief 从资源管理器中获取对应id的图像组件
    /// @param img_id 图像的唯一id
    /// @return 是否加载成功
    static bool create(GameObject& game_obj, const std::string& img_id);

    /// @brief 翻转图像组件
    static void flip(GameObject& game_obj);

    /// @brief 获取图像组件的大小
    static const Vector2& size(const GameObject& game_obj);

    /// @brief 设置图像组件的大小
    static void setSize(GameObject& game_obj, Vector2 size);

    /// @brief 大小等比缩放
    /// @param zoom_factor 缩放因子
    static void scaleProportionally(GameObject& game_obj, float zoom_factor);

    /// @brief 大小按照X, Y进行缩放
    /// @param zoom_factor
    static void scale(GameObject& game_obj, const Vector2& zoom_factor);
};

struct CollisionTool
{
    /// @brief 设置游戏对象的碰撞器的可用
    static bool setCollisionEnabled(GameObject& game_obj, bool enabled);

    /// @brief 设置游戏对象碰撞器触发后的回调
    static bool setOnCollide(GameObject& game_obj, std::function<void(GameObject&)> on_collide);

    /// @brief 设置游戏对象碰撞器的源层级
    static bool setSrcLayer(GameObject& game_obj, Collisionlayer src_layer);

    /// @brief 添加游戏对象碰撞器的目标层级
    static bool addDstLayer(GameObject& game_obj, Collisionlayer dst_layer);

    /// @brief 获取游戏对象碰撞组件的位置
    static Vector2 getCollisionPostion(GameObject& game_obj);

    /// @brief 操作游戏对象的碰撞box
    static bool modCollisionBox(GameObject& game_obj);

    /// @brief 操作游戏对象的碰撞box
    /// @param delta 和游戏对象之间的位移
    static bool modCollisionBox(GameObject& game_obj, Vector2 delta);

    /// @brief 操作游戏对象的碰撞box
    /// @param factor_x x方向的缩放因子
    /// @param factor_y y方向的缩放因子
    static bool modCollisionBox(GameObject& game_obj, float factor_x, float factor_y);

    /// @brief 操作游戏对象的碰撞box
    /// @param size 设置碰撞box的大小
    static bool setCollisionBoxSize(GameObject& game_obj, Vector2 size);

    /// @brief 获取游戏对象的碰撞box的大小
    static Vector2 getCollisionBoxSize(GameObject& game_obj);
};

}  // namespace gameaf