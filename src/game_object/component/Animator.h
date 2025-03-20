#pragma once

#include <game_object/component/Animation.h>
#include <game_object/component/Component.h>

#include <string>
#include <unordered_map>

/**
 * 动画管理类
 * 负责管理多种帧动画, 存放在一起方便切换, 以及统一设置管理
 */

namespace gameaf {

class Camera;
class Atlas;
class Animator : public Component
{
public:
    Animator(GameObject* obj);
    ~Animator();

public:
    void OnUpdate(float delta);
    void OnRender(const Camera& camera);
    void SetGameObject(GameObject* obj) override;
    void Flip() override;

public:
    /// @brief 添加动画对象来自图像(一张精灵图包含的所有图都属于同一个动画)
    /// @param animation_id 动画对象在当前动画管理器中的唯一id
    /// @param imgID 图元对象ID
    /// @param isLoop 是否循环
    /// @param interval 帧之间的间隔
    /// @return 是否添加成功
    bool AddAnimationForImage(const std::string& animation_id, const std::string& imgID,
                              bool isLoop = true, float interval = 0.15f);

    /// @brief 添加动画对象来自图像
    /// @param animation_id 动画对象在当前动画管理器中的唯一id
    /// @param imgID 图元对象ID
    /// @param indexs 指定精灵图中对应的图编号
    /// @param isLoop 是否循环
    /// @param interval 帧之间的间隔
    /// @return 是否添加成功
    bool AddAnimationForImage(const std::string& animation_id, const std::string& imgID,
                              const std::vector<size_t>& indexs, bool isLoop = true,
                              float interval = 0.15f);

    /// @brief 添加动画对象来自图像
    /// @param animation_id 动画对象在当前动画管理器中的唯一id
    /// @param imgID 图元对象ID
    /// @param begin 精灵图开始的编号
    /// @param len 精灵图的个数
    /// @param isLoop 是否循环
    /// @param interval 帧之间的间隔
    /// @return 是否添加成功
    bool AddAnimationForImage(const std::string& animation_id, const std::string& imgID,
                              size_t begin, size_t len, bool isLoop = true, float interval = 0.15f);

    /// @brief 添加动画对象来自图集
    /// @param animation_id 动画对象在当前动画管理器中的唯一id
    /// @param atlasID 图集对象ID
    /// @param isLoop 是否循环
    /// @param interval 帧之间的间隔
    /// @return 是否添加成功
    bool AddAnimationForAtlas(const std::string& animation_id, const std::string& atlasID,
                              bool isLoop = true, float interval = 0.15f);

    /// @brief 删除动画对象
    /// @param animation_id 动画对象id
    /// @return 是否删除成功
    bool DelAnimation(const std::string& animation_id);

    /// @brief 获取动画对象
    /// @param animation_id 动画对象id
    /// @return 动画对象
    Animation& GetAnimation(const std::string& animation_id);

    /// @brief 获取动画对象
    /// @param animation_id 动画对象id
    /// @return 动画对象
    const Animation& GetAnimation(const std::string& animation_id) const;

    /// @brief 获取当前的动画对象
    Animation& GetCurrentAnimation();

    /// @brief 设置默认的动画对象
    /// @note 默认情况下如果没调用此函数, 第一个加入进来的动画对象
    void SetInitialAnimation(const std::string& initial_animation_id);

    /// @brief 获取默认动画对象
    /// @return 默认动画对象
    const Animation& GetInitialAnimation() const;

    /// @brief 动画管理器切换动画
    /// @param animation_id 切换目标动画
    /// @return 切换是否成功
    bool SwitchToAnimation(const std::string& animation_id);

    bool ContainsScreenPoint(const Camera&, const Vector2&) const override;

    /// @brief 动画全部重置
    void Restart();

    /// @brief 设置所有动画的图像大小比例
    void SetSizeScale(const Vector2& scale);

    /// @brief 设置锚点模式
    /// @param mod 锚点模式
    /// @param anchor_position 如果自定义,
    /// 请自由的设置锚点在x方向所占的比例和在y方向所占的比例(单位矩形).值的取值在[0,1]
    void SetAnchorMode(ImageAnchorMode mod, const Vector2& anchor_position = {0.0f, 0.0f});

    /// @brief 新增过渡动画
    /// @param firstAnimationId 上一个动画名
    /// @param secondAnimationId 下一个动画名
    /// @param transitionAnimationId 过渡动画名
    bool AddTransitionAnimation(const std::string& firstAnimationId,
                                const std::string& secondAnimationId,
                                const std::string& transitionAnimationId);

private:
    std::unordered_map<std::string, Animation> m_animations;
    std::string m_current_animation_id;
    std::string m_initial_animation_id;

private:
    struct PairHash
    {
        template <typename T1, typename T2>
        size_t operator()(const std::pair<T1, T2>& p) const
        {
            return std::hash<T1>()(p.first) ^ (std::hash<T2>()(p.second) << 1);
        }
    };

    std::unordered_map<std::pair<std::string, std::string>, std::string, PairHash>
        m_transitionAnimations;     // 记载的过渡动画缓存
    bool m_isTransition = false;    // 是否过渡
    std::string m_nextAnimationId;  // 过渡状态后的下一个动画id
};
}  // namespace gameaf