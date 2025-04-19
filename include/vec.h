#pragma once
#include "constexpr_sqrt.h"

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <type_traits>

#define VEC_COMPONENT_ALIAS(name, index)                                                                               \
    auto& name()                                                                                                       \
    {                                                                                                                  \
        return this->components[index];                                                                                \
    }                                                                                                                  \
    const auto& name() const                                                                                           \
    {                                                                                                                  \
        return this->components[index];                                                                                \
    }

template<typename T, std::size_t N>
struct Vec
{
    std::array<T, N> components;

    constexpr Vec();
    constexpr Vec(const std::array<T, N>& components);

    template<typename... Args, typename = typename std::enable_if<sizeof...(Args) == N>::type>
    constexpr Vec(Args... args);

    template<typename U>
    constexpr operator Vec<U, N>() const;

    constexpr T& operator[](std::size_t index);
    constexpr const T& operator[](std::size_t index) const;

    constexpr bool operator==(const Vec& v) const = default;
    constexpr bool operator!=(const Vec& v) const = default;
    constexpr Vec operator-() const;
    constexpr Vec operator+(const Vec& v) const;
    constexpr Vec operator-(const Vec& v) const;
    constexpr Vec operator*(T s) const;
    constexpr Vec operator/(T s) const;
    constexpr Vec& operator+=(const Vec& v);
    constexpr Vec& operator-=(const Vec& v);
    constexpr Vec& operator*=(T s);
    constexpr Vec& operator/=(T s);

    [[nodiscard]] constexpr double dot(const Vec& v) const;
    [[nodiscard]] constexpr double len() const;
    [[nodiscard]] constexpr Vec norm() const;
    [[nodiscard]] constexpr Vec<T, N> perp() const
    requires(N == 2);
    [[nodiscard]] constexpr Vec<T, N> cross(const Vec<T, N>& v) const
    requires(N == 3);
    [[nodiscard]] constexpr double dist(const Vec& v) const;
    [[nodiscard]] constexpr double dist_sq(const Vec& v) const;
    [[nodiscard]] constexpr double angle(const Vec& v) const;
    [[nodiscard]] constexpr Vec<T, N> rotate(double angle) const
    requires(N == 2);
    [[nodiscard]] constexpr Vec<T, N> rotate(const Vec<T, N>& axis, double angle) const
    requires(N == 3);
    [[nodiscard]] constexpr Vec<T, N> lerp(const Vec<T, N>& v, double t) const;
};

template<typename T>
using Vec2 = Vec<T, 2>;
template<typename T>
using Vec3 = Vec<T, 3>;
template<typename T>
using Vec4 = Vec<T, 4>;

template<typename T, std::size_t N>
constexpr Vec<T, N>::Vec()
{
    components.fill(T(0));
}

template<typename T, std::size_t N>
constexpr Vec<T, N>::Vec(const std::array<T, N>& components)
    : components(components)
{
}

template<typename T, std::size_t N>
template<typename... Args, typename>
constexpr Vec<T, N>::Vec(Args... args)
    : components{ static_cast<T>(args)... }
{
}

template<typename T, std::size_t N>
template<typename U>
constexpr Vec<T, N>::operator Vec<U, N>() const
{
    std::array<U, N> converted;
    for (std::size_t i = 0; i < N; ++i) {
        converted[i] = static_cast<U>(components[i]);
    }
    return Vec<U, N>(converted);
}

template<typename T, std::size_t N>
constexpr T& Vec<T, N>::operator[](std::size_t index)
{
    assert(index < N && "Index out of bounds");
    return components[index];
}

template<typename T, std::size_t N>
constexpr const T& Vec<T, N>::operator[](std::size_t index) const
{
    assert(index < N && "Index out of bounds");
    return components[index];
}

template<typename T, std::size_t N>
constexpr Vec<T, N> Vec<T, N>::operator-() const
{
    Vec<T, N> result;
    for (std::size_t i = 0; i < N; ++i) {
        result.components[i] = -components[i];
    }
    return result;
}

template<typename T, std::size_t N>
constexpr Vec<T, N> Vec<T, N>::operator+(const Vec<T, N>& v) const
{
    Vec<T, N> result;
    for (std::size_t i = 0; i < N; ++i) {
        result.components[i] = components[i] + v.components[i];
    }
    return result;
}

template<typename T, std::size_t N>
constexpr Vec<T, N> Vec<T, N>::operator-(const Vec<T, N>& v) const
{
    Vec<T, N> result;
    for (std::size_t i = 0; i < N; ++i) {
        result.components[i] = components[i] - v.components[i];
    }
    return result;
}

template<typename T, std::size_t N>
constexpr Vec<T, N> Vec<T, N>::operator*(T s) const
{
    Vec<T, N> result;
    for (std::size_t i = 0; i < N; ++i) {
        result.components[i] = components[i] * s;
    }
    return result;
}

template<typename T, std::size_t N>
constexpr Vec<T, N> Vec<T, N>::operator/(T s) const
{
    if (s == 0) {
        throw std::runtime_error("Division by zero");
    }
    Vec<T, N> result;
    for (std::size_t i = 0; i < N; ++i) {
        result.components[i] = components[i] / s;
    }
    return result;
}

template<typename T, std::size_t N>
constexpr Vec<T, N>& Vec<T, N>::operator+=(const Vec<T, N>& v)
{
    for (std::size_t i = 0; i < N; ++i) {
        components[i] += v.components[i];
    }
    return *this;
}

template<typename T, std::size_t N>
constexpr Vec<T, N>& Vec<T, N>::operator-=(const Vec<T, N>& v)
{
    for (std::size_t i = 0; i < N; ++i) {
        components[i] -= v.components[i];
    }
    return *this;
}

template<typename T, std::size_t N>
constexpr Vec<T, N>& Vec<T, N>::operator*=(T s)
{
    for (std::size_t i = 0; i < N; ++i) {
        components[i] *= s;
    }
    return *this;
}

template<typename T, std::size_t N>
constexpr Vec<T, N>& Vec<T, N>::operator/=(T s)
{
    if (s == 0) {
        throw std::runtime_error("Division by zero");
    }
    for (std::size_t i = 0; i < N; ++i) {
        components[i] /= s;
    }
    return *this;
}

template<typename T, std::size_t N>
constexpr double Vec<T, N>::dot(const Vec<T, N>& v) const
{
    double result = 0;
    for (std::size_t i = 0; i < N; ++i) {
        result += components[i] * v.components[i];
    }
    return result;
}

template<typename T, std::size_t N>
constexpr double Vec<T, N>::len() const
{
    return constexpr_sqrt(dot(*this));
}

template<typename T, std::size_t N>
constexpr Vec<T, N> Vec<T, N>::norm() const
{
    double length = len();
    if (length == 0) {
        return Vec<T, N>();
    }
    return *this / length;
}

template<typename T, std::size_t N>
constexpr Vec<T, N> Vec<T, N>::perp() const
requires(N == 2)
{
    return Vec<T, N>{ -components[1], components[0] };
}

template<typename T, std::size_t N>
constexpr Vec<T, N> Vec<T, N>::cross(const Vec<T, N>& v) const
requires(N == 3)
{
    return Vec<T, N>{ (components[1] * v.components[2]) - (components[2] * v.components[1]),
                      (components[2] * v.components[0]) - (components[0] * v.components[2]),
                      (components[0] * v.components[1]) - (components[1] * v.components[0]) };
}

template<typename T, std::size_t N>
constexpr double Vec<T, N>::dist(const Vec<T, N>& v) const
{
    return constexpr_sqrt(dist_sq(v));
}

template<typename T, std::size_t N>
constexpr double Vec<T, N>::dist_sq(const Vec<T, N>& v) const
{
    double result = 0;
    for (std::size_t i = 0; i < N; ++i) {
        double diff = components[i] - v.components[i];
        result += diff * diff;
    }
    return result;
}

template<typename T, std::size_t N>
constexpr double Vec<T, N>::angle(const Vec<T, N>& v) const
{
    double dot_product = dot(v);
    double lengths = len() * v.len();
    if (lengths == 0) {
        return 0;
    }
    return std::acos(dot_product / lengths);
}

template<typename T, std::size_t N>
constexpr Vec<T, N> Vec<T, N>::rotate(double angle) const
requires(N == 2)
{
    double cos_angle = std::cos(angle);
    double sin_angle = std::sin(angle);
    return Vec<T, N>{ (components[0] * cos_angle) - (components[1] * sin_angle),
                      (components[0] * sin_angle) + (components[1] * cos_angle) };
}

template<typename T, std::size_t N>
constexpr Vec<T, N> Vec<T, N>::rotate(const Vec<T, N>& axis, double angle) const
requires(N == 3)
{
    double cos_angle = std::cos(angle);
    double sin_angle = std::sin(angle);
    return (*this * cos_angle) + (axis.cross(*this) * sin_angle) + (axis * axis.dot(*this) * (1 - cos_angle));
}

template<typename T, std::size_t N>
constexpr Vec<T, N> Vec<T, N>::lerp(const Vec<T, N>& v, double t) const
{
    return *this + ((v - *this) * t);
}