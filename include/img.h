#pragma once
#include "array2d.h"
#include "vec.h"

#include <string>

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

class Img
{
private:
    Array2d<Color> arr;

public:
    Img(size_t w = 0, size_t h = 1);
    Img(const std::string& path);
    Array2d<Color>& operator*();
    Array2d<Color>* operator->();
    const Array2d<Color>& operator*() const;
    const Array2d<Color>* operator->() const;
    void save(const std::string& path);
};