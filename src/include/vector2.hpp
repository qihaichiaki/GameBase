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
};

inline std::ostream& operator<<(std::ostream& os, const Vector2& vec)
{
    os << '(' << vec.X << ", " << vec.Y << ")";
    return os;
}
}  // namespace gameaf