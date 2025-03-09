#pragma once

#include <algorithm>
#include <ostream>

namespace gameaf {

class Vector2
{
public:
    float X = 0;
    float Y = 0;

public:
    Vector2() = default;
    ~Vector2() = default;
    Vector2(float x, float y) : X(x), Y(y) {}

public:
    Vector2 operator+(const Vector2& vec) const { return Vector2(X + vec.X, Y + vec.Y); }
    void operator+=(const Vector2& vec)
    {
        X += vec.X;
        Y += vec.Y;
    }
    Vector2 operator-(const Vector2& vec) const { return Vector2(X - vec.X, Y - vec.Y); }
    Vector2 operator/(float num) const { return Vector2(X / num, Y / num); }
    Vector2 operator*(float num) const { return Vector2(X * num, Y * num); }
    void operator*=(float num)
    {
        X *= num;
        Y *= num;
    }
    Vector2 operator*(const Vector2& vec) const { return Vector2(X * vec.X, Y * vec.Y); }
    void operator*=(const Vector2& vec)
    {
        X *= vec.X;
        Y *= vec.Y;
    }
    bool operator==(const Vector2& vec) const { return X == vec.X && Y == vec.Y; }
    bool operator!=(const Vector2& vec) const { return !(*this == vec); }

    float length() const { return std::sqrt(X * X + Y * Y); }

public:
    static Vector2 lerp(const Vector2& v_start, const Vector2& v_end, float smoothing)
    {
        smoothing = std::clamp(smoothing, 0.0f, 1.0f);
        // 插值`缓慢->加速->缓慢`的效果
        smoothing = smoothing * smoothing * (3 - 2 * smoothing);  // SmoothStep
        // smoothing = smoothing * smoothing * smoothing *
        //             (smoothing * (smoothing * 6 - 15) + 10);  // SmootherStep
        return v_start + (v_end - v_start) * smoothing;
    }
};

inline std::ostream& operator<<(std::ostream& os, const Vector2& vec)
{
    os << '(' << vec.X << ", " << vec.Y << ")";
    return os;
}
}  // namespace gameaf