#pragma once
#include "array2d.h"
#include "color.h"

#include <string>

class Img : public Array2d<Color>
{
public:
    Img(size_t w = 0, size_t h = 1, Color color = Color(0.0, 0.0, 0.0, 0.0));
    static Img load(const std::string& path);
    void save(const std::string& path);

    Img operator+(const Img& other) const;
    Img operator+(const Color& color) const;
    Img& operator+=(const Img& other);
    Img& operator+=(const Color& color);
};