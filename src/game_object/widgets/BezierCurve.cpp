#include "BezierCurve.h"

#include <common/MediaUtils.hpp>

namespace gameaf {

BezierCurve::BezierCurve(Vector2 p0, Vector2 p1, Vector2 p2)
    : GameObject{"BezierCurve"}, m_p0(p0), m_p1(p1), m_p2(p2)
{
    BuildPoints();
}

Vector2 BezierCurve::Lerp(float t)
{
    return _Lerp(t) + GetPosition();  // 二阶贝塞尔公式
}

Vector2 BezierCurve::_Lerp(float t)
{
    float u = 1 - t;
    float tt = t * t;
    float uu = u * u;
    return m_p0 * uu + m_p1 * u * t * 2.0f + m_p2 * tt;  // 二阶贝塞尔公式
}

void BezierCurve::BuildPoints()
{
    m_points.clear();
    m_points.reserve(m_steps);
    m_points.push_back(m_p0);
    float t;
    for (int i = 1; i < m_steps; ++i) {
        t = i * 1.0f / m_steps;
        m_points.push_back(_Lerp(t));
    }
    m_points.push_back(m_p2);
}

void BezierCurve::SetP0(const Vector2& p0)
{
    if (!m_p0.IsEquals(p0)) {
        m_p0 = p0;
        BuildPoints();
    }
}

void BezierCurve::SetP1(const Vector2& p1)
{
    if (!m_p1.IsEquals(p1)) {
        m_p1 = p1;
        BuildPoints();
    }
}

void BezierCurve::SetP2(const Vector2& p2)
{
    if (!m_p2.IsEquals(p2)) {
        m_p2 = p2;
        BuildPoints();
    }
}

void BezierCurve::OnDraw(const Camera& camera)
{
    Vector2 cameraPos = camera.GetPosition();
    Vector2 offset = GetPosition();
#ifdef GAMEAF_USE_EASYX
    setcolor(ColorRGB{0, 255, 0});  // 绿色

    Vector2 pre = (m_p0 + offset);
    for (int i = 1; i <= m_steps; ++i) {
        Vector2 current = (m_points[i] + offset);
        line(pre.X - cameraPos.X, pre.Y - cameraPos.Y, current.X - cameraPos.X,
             current.Y - cameraPos.Y);  // 画线
        pre = current;                  // 更新点
    }
#else
#endif
}

}  // namespace gameaf