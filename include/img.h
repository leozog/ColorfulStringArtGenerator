#pragma once
#include <string>
#include <stdint.h>
#include <limits>
#include <type_traits>
#include "array2d.h"

struct color
{
    static constexpr uint8_t CHANNELS = 4;
    float r, g, b, a;

    color(double r, double g, double b, double a = 1.0f);
    color(int r = 0, int g = 0, int b = 0, int a = 255);
    color operator+(const color &x) const;
    color operator*(const double x) const;
    color &operator+=(const color &x);
    color &operator*=(const double x);
    color clamp() const;
};

class img
{
    array2d<color> arr;

public:
    img(size_t w = 1, size_t h = 1);
    img(std::string name);
    array2d<color> &operator*();
    array2d<color> *operator->();
    void save(std::string name);
};