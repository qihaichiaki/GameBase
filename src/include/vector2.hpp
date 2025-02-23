#pragma once

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

    float length() const { return std::sqrt(X * X + Y * Y); }

public:
    static Vector2 lerp(const Vector2& v_start, const Vector2& v_end, float smoothing)
    {
        if (smoothing < 0.0f) smoothing = 0.0f;
        if (smoothing > 1.0f) smoothing = 1.0f;
        return v_start + (v_end - v_start) * smoothing;
    }
};

inline std::ostream& operator<<(std::ostream& os, const Vector2& vec)
{
    os << '(' << vec.X << ", " << vec.Y << ")";
    return os;
}
}  // namespace gameaf