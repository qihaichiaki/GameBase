#pragma once

#include <common/Vector2.hpp>
#include <memory>
#include <string>
#include <vector>

namespace gameaf {

// 前置声明
class Component;
class Image;
class Animator;
class Scene;
class Camera;
class Collision;
class CollisionBox;
class Rigidbody2D;
class Text;

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
    // using
    using GameObjectPtr = std::shared_ptr<GameObject>;
    using GameObjectWeakPtr = GameObject*;
    using SceneWeakPtr = std::weak_ptr<Scene>;
    using ChildGameObjects = std::unique_ptr<std::vector<GameObjectPtr>>;
    using ImagePtr = std::unique_ptr<Image>;  // image的生命周期由资源管理器管理
    using AnimatorPtr = std::unique_ptr<Animator>;
    using CollisionPtr = Collision*;  // 碰撞组件的生命周期由碰撞管理器管理
    using CollisionList = std::unique_ptr<std::vector<CollisionPtr>>;
    using Rigidbody2DPtr = std::unique_ptr<Rigidbody2D>;
    using TextPtr = std::unique_ptr<Text>;
    using TextLists = std::unique_ptr<std::vector<TextPtr>>;
    using CameraLists = std::unique_ptr<std::vector<Camera*>>;

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

    /// @brief 每个游戏帧都会调用一次
    virtual void OnUpdate() { m_updateCallback ? m_updateCallback(this) : 0; }

    /// @brief 每个物理帧都会调用一次
    virtual void OnFixUpdate() {}

    /// @brief 每个渲染帧会调用一次, 可以添加一些组件的debug渲染
    virtual void OnDraw(const Camera&) {}

    /// @brief 每当加载入场景时调用一次, clone时不调用
    virtual void OnAwake() {}

    // 鼠标事件

    /// @brief 鼠标进入对象区域
    virtual void OnMouseEnter() {}

    /// @brief 鼠标离开对象区域
    virtual void OnMouseExit() {}

    /// @brief 鼠标点击
    virtual void OnMouseClicked() {}

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
    virtual GameObjectPtr Clone()
    {
        auto clonePtr = std::make_shared<GameObject>(*this);
        if (auto parent = GetParent()) {
            parent->AddChildObject(clonePtr);
        }
        return clonePtr;
    }

    /// @brief 获取当前游戏对象的父对象
    GameObject* GetParent() const { return m_parent; }

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

    /// @brief 根据对象id取出子游戏对象(们)
    std::vector<GameObjectPtr> FindChildObjects(const std::string& child_id);

    /// @brief 根据对象id删除对应子游戏对象(们)
    void DelChildObjects(const std::string& child_id);

    // 水平翻转组件
    void Flip();

    /// @brief 设置游戏对象是否活动, 如果不活动则后续不会调用其update、render相关方法
    void SetActive(bool isActive)
    {
        if (!m_isActive && isActive) {
            OnEnter();
        }
        m_isActive = isActive;
    }

    /// @brief 查看游戏对象是否处于活跃状态
    bool GetActive() const { return m_isActive; }

    // 创建组件
    template <typename T, typename... Args>
    std::enable_if_t<std::is_base_of_v<Component, T>, T*> CreateComponent(Args&&... args);

    // 获取组件指针
    template <typename T>
    std::enable_if_t<std::is_base_of_v<Component, T>, T*> GetComponent(int index = 0);

public:
    /// @brief 游戏框架内置物理更新
    /// @param alpha 物理固定更新频率
    void OnFixUpdate(float alpha);
    /// @brief 游戏框架内置更新
    /// @param delta 当前帧和上一帧相差时间间隔
    void OnUpdate(float delta);
    /// @brief 游戏框架内置渲染
    void OnRender(const Camera&);

    /// @brief 附加摄像机对象
    void AttachCamera(Camera*);
    /// @brief 判断一个窗口坐标是否在对象内部
    bool ContainsScreenPoint(const Vector2& pos);

private:
    /// @brief 根据对象地址删除对应的子对象
    void DetachChildObject(GameObject*);

    /// @brief 转移资源
    void Swap(GameObject&);

    /// @brief 返回当前渲染该游戏对象的摄像机（子游戏对象返回的是父游戏对象的）
    const CameraLists& GetCameras();

private:
    // === 固有属性 ===
    // 游戏对象名称
    std::string m_name = "GameObject";
    // 渲染层级 值越大后渲染，值越小先渲染
    int m_zOrder = 0;
    // gameobject所属的scene
    // scene的生命周期不由gameobject进行管理
    SceneWeakPtr m_myScene;
    bool m_isAwake = false;  // 判断是否唤醒过, 一个游戏对象只能一次
    bool m_isActive = true;  // 是否活动, 如果不活动，则不对其对象进行渲染和update

    // transform
    Vector2 m_position;  // 位置组件

    // === 特殊属性(可有可无, 使用指针管理,延迟加载) ===
    ImagePtr m_image = nullptr;              // 图像组件
    AnimatorPtr m_animator = nullptr;        // 动画管理器组件
    CollisionList m_collisions = nullptr;    // 碰撞组件
    Rigidbody2DPtr m_rigidbody2D = nullptr;  // 刚体组件
    TextLists m_texts = nullptr;             // 文本组件
    // ...

    ChildGameObjects m_child_gameObjects = nullptr;  // 子游戏对象容器
    GameObjectWeakPtr m_parent = nullptr;            // 父对象
    CameraLists m_cameras;                           // 当前游戏对象附加的摄像机对象们
};
}  // namespace gameaf