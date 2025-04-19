#include "color.h"
#include <algorithm>

Color::Color(double r, double g, double b, double a)
    : Vec(r, g, b, a)
{
}

Color::Color(int r, int g, int b, int a)
    : Color{ static_cast<float>(r) / 255.f,
             static_cast<float>(g) / 255.f,
             static_cast<float>(b) / 255.f,
             static_cast<float>(a) / 255.f }
{
}

Color Color::operator+(const Color& x) const
{
    double ao = x.a() + (a() * (1. - x.a()));
    return { (x.r() * x.a() + r() * a() * (1 - x.a())) / ao,
             (x.g() * x.a() + g() * a() * (1 - x.a())) / ao,
             (x.b() * x.a() + b() * a() * (1 - x.a())) / ao,
             ao };
}

Color Color::operator-(const Color& x) const
{
    return this->operator+(Color{ -x.r(), -x.g(), -x.b(), x.a() });
}

Color& Color::operator+=(const Color& x)
{
    *this = *this + x;
    return *this;
}

Color& Color::operator-=(const Color& x)
{
    *this = *this - x;
    return *this;
}

Color Color::clamp() const
{
    return {
        std::clamp(r(), 0.f, 1.f), std::clamp(g(), 0.f, 1.f), std::clamp(b(), 0.f, 1.f), std::clamp(a(), 0.f, 1.f)
    };
}