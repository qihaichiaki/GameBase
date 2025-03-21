#pragma once

#include <game_object/GameObject.h>

/**
 * 贝塞尔曲线, 基于三个控制点的参数曲线, 方便绘制轨迹等
 */

namespace gameaf {
class BezierCurve : public GameObject
{
public:
    /// @brief 创建一条二阶的贝塞尔曲线
    /// @param p0 起点
    /// @param p1 控制点
    /// @param p2 终点
    /// @note 这些点都是相对于对象pos的, 意思是贝塞尔曲线的最终点都是建立在 p0 + gameObject->pos() ...
    BezierCurve(Vector2 p0, Vector2 p1, Vector2 p2);

    /// @brief 贝塞尔曲线插值
    /// @param t 进度
    /// @return 对应进度上的点坐标
    Vector2 Lerp(float t);

    /// @brief 设置起点
    void SetP0(const Vector2& p0);

    /// @brief 设置控制点
    void SetP1(const Vector2& p1);

    /// @brief 设置终点
    void SetP2(const Vector2& p2);

    /// @brief 获取起点
    Vector2 GetP0() const { return m_p0; }

    /// @brief 获取控制点
    Vector2 GetP1() const { return m_p1; }

    /// @brief 获取终点
    Vector2 GetP2() const { return m_p2; }

public:
    GameObjectPtr Clone() override
    {
        auto clonePtr = std::make_shared<BezierCurve>(*this);
        if (auto parent = GetParent()) {
            parent->AddChildObject(clonePtr);
        }
        return clonePtr;
    }

    // void OnUpdate() override;

    void OnDraw(const Camera& camera) override;

private:
    void BuildPoints();      // 更新点的缓存
    Vector2 _Lerp(float t);  // 不计算偏移的插值

private:
    // 公共属性
    Vector2 m_p0;  // 起点
    Vector2 m_p1;  // 控制点1 控制曲线的弯曲程度
    Vector2 m_p2;  // 终点

    int m_steps = 50;               // 缓存点个数
    std::vector<Vector2> m_points;  // 缓存点, 方便绘图
};

}  // namespace gameaf