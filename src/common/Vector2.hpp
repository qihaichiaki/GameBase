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
    bool IsEquals(const Vector2& other, float epsilon = 1e-5f) const
    {
        return std::abs(X - other.X) < epsilon && std::abs(Y - other.Y) < epsilon;
    }
    Vector2 operator+(float num) const { return Vector2(X + num, Y + num); }
    void operator+=(float num)
    {
        X += num;
        Y += num;
    }
    Vector2 operator+(const Vector2& vec) const { return Vector2(X + vec.X, Y + vec.Y); }
    void operator+=(const Vector2& vec)
    {
        X += vec.X;
        Y += vec.Y;
    }
    Vector2 operator-(float num) const { return Vector2(X - num, Y - num); }
    void operator-=(float num)
    {
        X -= num;
        Y -= num;
    }
    Vector2 operator-(const Vector2& vec) const { return Vector2(X - vec.X, Y - vec.Y); }
    void operator-=(const Vector2& vec)
    {
        X -= vec.X;
        Y -= vec.Y;
    }
    Vector2 operator/(float num) const { return Vector2(X / num, Y / num); }
    Vector2 operator/=(float num)
    {
        X /= num;
        Y /= num;
    }
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
    bool operator==(const Vector2& vec) const { return IsEquals(vec); }
    bool operator!=(const Vector2& vec) const { return !(*this == vec); }

    float Length() const { return std::sqrt(X * X + Y * Y); }

    void Normalize()
    {
        float len = std::sqrt(X * X + Y * Y);
        if (len > 0.0f) {
            X /= len;
            Y /= len;
        }
    }

    Vector2 Normalized() const
    {
        Vector2 result = *this;
        result.Normalize();
        return result;
    }

    operator bool() { return X != 0.0f || Y != 0.0f; }

public:
    static Vector2 Lerp(const Vector2& v_start, const Vector2& v_end, float smoothing)
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