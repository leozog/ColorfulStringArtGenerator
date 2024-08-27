#pragma once

#include <array>
#include <cstddef>
#include <cmath>

template <typename T>
struct Vec2
{
    T x, y;

    Vec2(T x = 0, T y = 0);
    template <typename U>
    operator Vec2<U>() const;
    Vec2 operator+(const Vec2 &v) const;
    Vec2 operator-(const Vec2 &v) const;
    Vec2 operator*(T s) const;
    Vec2 operator/(T s) const;
    Vec2 &operator+=(const Vec2 &v);
    Vec2 &operator-=(const Vec2 &v);
    Vec2 &operator*=(T s);
    Vec2 &operator/=(T s);
    double dot(const Vec2 &v) const;
    double len() const;
    Vec2 norm() const;
    Vec2 perp() const;
};

template <typename T>
Vec2<T>::Vec2(T x, T y) : x(x), y(y) {}

template <typename T>
template <typename U>
inline Vec2<T>::operator Vec2<U>() const
{
    return Vec2<U>(static_cast<U>(x), static_cast<U>(y));
}

template <typename T>
Vec2<T> Vec2<T>::operator+(const Vec2<T> &v) const
{
    return Vec2<T>(x + v.x, y + v.y);
}

template <typename T>
Vec2<T> Vec2<T>::operator-(const Vec2<T> &v) const
{
    return Vec2<T>(x - v.x, y - v.y);
}

template <typename T>
Vec2<T> Vec2<T>::operator*(T s) const
{
    return Vec2<T>(x * s, y * s);
}

template <typename T>
Vec2<T> Vec2<T>::operator/(T s) const
{
    return Vec2<T>(x / s, y / s);
}

template <typename T>
Vec2<T> &Vec2<T>::operator+=(const Vec2<T> &v)
{
    x += v.x;
    y += v.y;
    return *this;
}

template <typename T>
Vec2<T> &Vec2<T>::operator-=(const Vec2<T> &v)
{
    x -= v.x;
    y -= v.y;
    return *this;
}

template <typename T>
Vec2<T> &Vec2<T>::operator*=(T s)
{
    x *= s;
    y *= s;
    return *this;
}

template <typename T>
Vec2<T> &Vec2<T>::operator/=(T s)
{
    x /= s;
    y /= s;
    return *this;
}

template <typename T>
double Vec2<T>::dot(const Vec2<T> &v) const
{
    return x * v.x + y * v.y;
}

template <typename T>
double Vec2<T>::len() const
{
    return std::sqrt(x * x + y * y);
}

template <typename T>
Vec2<T> Vec2<T>::norm() const
{
    return *this / len();
}

template <typename T>
Vec2<T> Vec2<T>::perp() const
{
    return Vec2<T>(-y, x);
}
