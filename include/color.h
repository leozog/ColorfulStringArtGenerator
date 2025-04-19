#pragma once
#include "vec.h"

struct Color : public Vec<float, 4>
{
    static constexpr int CHANNELS = 4;

    VEC_COMPONENT_ALIAS(r, 0);
    VEC_COMPONENT_ALIAS(g, 1);
    VEC_COMPONENT_ALIAS(b, 2);
    VEC_COMPONENT_ALIAS(a, 3);

    Color(double r, double g, double b, double a = 1.0f);
    Color(int r = 0, int g = 0, int b = 0, int a = 255);
    Color operator+(const Color& x) const;
    Color operator-(const Color& x) const;
    Color& operator+=(const Color& x);
    Color& operator-=(const Color& x);
    Color clamp() const;
};