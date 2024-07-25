#pragma once
#include <string>
#include <stdint.h>
#include <limits>
#include <type_traits>
#include "array2d.h"

struct Color
{
    static constexpr uint8_t CHANNELS = 4;
    float r, g, b, a;

    Color(int r = 0, int g = 0, int b = 0, int a = 255);
    Color(double r, double g, double b, double a = 1.0f);
    Color operator+(const Color &x) const;
    Color operator*(const double x) const;
    Color &operator+=(const Color &x);
    Color &operator*=(const double x);
    Color clamp() const;
};

class Img
{
private:
    Array2d<Color> arr;

public:
    Img(size_t w = 0, size_t h = 1);
    Img(std::string name);
    Array2d<Color> &operator*();
    Array2d<Color> *operator->();
    const Array2d<Color> &operator*() const;
    const Array2d<Color> *operator->() const;
    void save(std::string name);
};