#pragma once

#include <common/Vector2.hpp>
#include <memory>
#include <string>
#include <vector>

namespace gameaf {

// 前置声明
class Image;
class Animator;
class Scene;
class Camera;
class Collision;
class CollisionBox;
class Rigidbody2D;

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
    // using
    using GameObjectPtr = std::shared_ptr<GameObject>;
    using GameObjectWeakPtr = std::weak_ptr<GameObject>;
    using SceneWeakPtr = std::weak_ptr<Scene>;
    using ChildGameObjects = std::unique_ptr<std::vector<GameObjectPtr>>;
    using ImagePtr = Image*;  // image的生命周期由资源管理器管理
    using AnimatorPtr = std::unique_ptr<Animator>;
    using CollisionPtr = Collision*;      // 碰撞组件的生命周期由碰撞管理器管理
    using Rigidbody2DPtr = Rigidbody2D*;  // 2d刚体组件的生命周期由碰撞管理器管理

public:
    friend class Scene;

public:
    GameObject();
    GameObject(const std::string& name);
    GameObject(int z_order, const std::string& name = "GameObject");
    /// @warning 注意拷贝构造后, 如果是子对象, 父对象并没有添加新的子对象, 需要在外额外进行添加
    GameObject(const GameObject&);
    GameObject(GameObject&&);
    GameObject& operator=(const GameObject&);
    GameObject& operator=(GameObject&&);

    virtual ~GameObject();

public:
    /// @brief 游戏对象加载入scene会进行调用
    virtual void OnEnter() { m_enterCallback ? m_enterCallback(this) : 0; }
    virtual void OnUpdate() { m_updateCallback ? m_updateCallback(this) : 0; }

    /// 注入回调式更新游戏对象
    using EnterCallback = void (*)(GameObject*);
    using UpdateCallback = void (*)(GameObject*);

    /// @brief 注入update调用依赖.
    /// @warning 如果当前对象为派生类对象, 重写的update函数中没有调用基类的update则依赖失效.
    void AddUpdateCallback(UpdateCallback callback) { m_updateCallback = callback; }
    /// @brief 注入enter调用依赖.
    /// @warning 如果当前对象为派生类对象, 重写的enter函数中没有调用基类的update则依赖失效.
    void AddEnterCallback(EnterCallback callback) { m_enterCallback = callback; }

private:
    EnterCallback m_enterCallback = nullptr;
    UpdateCallback m_updateCallback = nullptr;

public:
    /// @brief 克隆一个一致的对象
    /// @warning 注意派生类中的属性并没有复制, 需要自己手动继承实现
    /// @note 此clone会自动继承父游戏对象
    virtual GameObjectPtr Clone();

    /// @brief 获取当前对象名字
    std::string GetName() const { return m_name; }

    /// @brief 设置游戏对象的名称
    void SetName(const std::string& name) { m_name = name; }

    /// @brief 设置当前对象的世界坐标
    void SetPosition(const Vector2& position) { m_position = position; }

    /// @brief 设置当前对象的X坐标
    void SetPositionX(float x) { m_position.X = x; }

    /// @brief 设置当前对象的Y坐标
    void SetPositionY(float y) { m_position.Y = y; }

    /// @brief 获取当前对象的世界坐标
    const Vector2& GetPosition() const { return m_position; }

    /// @brief 游戏对象位置进行位移
    /// @param offset 位移大小
    void Translate(const Vector2& offset) { m_position += offset; }

    /// @brief 设置渲染层级,越大渲染级别越高,越后被渲染
    void SetZOrder(int z_order);

    /// @brief 获得渲染层级,越大渲染级别越高,越后被渲染
    int GetZOrder() const { return m_zOrder; }

    /// @brief 添加子对象
    /// @note - 父对象会添加为当前对象, 并且如果之前存在父对象, 则会遍历删除一遍
    /// @note - 成为子对象, 渲染级别和父对象一致(子对象之间看插入的先后关系)
    void AddChildObject(GameObjectPtr child);

    /// @brief 添加多个子对象
    void AddChildObjects(const std::vector<GameObjectPtr>& children);

    /// @brief 指定取出第几个子游戏对象
    GameObjectPtr GetChildObject(size_t index);

    /// @brief 根据对象id取出对象(们)
    std::vector<GameObjectPtr> FindChildObjects(const std::string& child_id);
    // TODO: 删除子游戏对象

    // 创建组件
    template <typename T, typename... Args>
    std::enable_if_t<std::is_same_v<T, Image> || std::is_same_v<T, Animator> ||
                         std::is_same_v<T, CollisionBox> || std::is_same_v<T, Rigidbody2D>,
                     T*>
    CreateComponent(Args&&... args);

    // 获取组件指针
    template <typename T>
    T* GetComponent();

public:
    /// @brief 游戏框架内置更新
    /// @param delta 当前帧和上一帧相差时间间隔
    void OnUpdate(float delta);
    /// @brief 游戏框架内置渲染
    void OnRender(const Camera&);

private:
    /// @brief 根据对象地址删除对应的子对象
    void DetachChildObject(GameObject*);

    /// @brief 转移资源
    void Swap(GameObject&);

private:
    // === 固有属性 ===
    // 游戏对象名称
    std::string m_name = "GameObject";
    // 渲染层级 值越大后渲染，值越小先渲染
    int m_zOrder = 0;
    // gameobject所属的scene
    // scene的生命周期不由gameobject进行管理
    SceneWeakPtr m_myScene;
    Vector2 m_position;  // 位置组件

    // === 特殊属性(可有可无, 使用指针管理,延迟加载) ===
    ImagePtr m_image = nullptr;              // 图像组件
    AnimatorPtr m_animator = nullptr;        // 动画管理器组件
    CollisionPtr m_collision = nullptr;      // 碰撞组件
    Rigidbody2DPtr m_rigidbody2D = nullptr;  // 刚体组件
    // ...

    ChildGameObjects m_child_gameObjects = nullptr;  // 子游戏对象容器
    GameObjectWeakPtr m_parent;                      // 父对象
};
}  // namespace gameaf