#include "img.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include <algorithm>

// color
Color::Color(int r, int g, int b, int a) : r{r / 255.f}, g{g / 255.f}, b{b / 255.f}, a{a / 255.f} {}

Color::Color(double r, double g, double b, double a) : r{static_cast<float>(r)}, g{static_cast<float>(g)}, b{static_cast<float>(b)}, a{static_cast<float>(a)} {}

Color Color::operator+(const Color &x) const
{
    float ao = x.a + a * (1. - x.a);
    return Color(
        (x.r * x.a + r * a * (1 - x.a)) / ao,
        (x.g * x.a + g * a * (1 - x.a)) / ao,
        (x.b * x.a + b * a * (1 - x.a)) / ao,
        ao);
}

Color Color::operator*(const double x) const
{
    return Color(
        r * x,
        g * x,
        b * x,
        a);
}

Color &Color::operator+=(const Color &x)
{
    *this = *this + x;
    return *this;
}

Color &Color::operator*=(const double x)
{
    *this = *this * x;
    return *this;
}

Color Color::clamp() const
{
    return Color(
        std::clamp(r, 0.f, 1.f),
        std::clamp(g, 0.f, 1.f),
        std::clamp(b, 0.f, 1.f),
        std::clamp(a, 0.f, 1.f));
}
// color

// img
Img::Img(size_t w, size_t h) : arr(w, h)
{
}

Img::Img(std::string name)
{
    int w, h, n;
    uint8_t *data = stbi_load(name.c_str(), &w, &h, &n, Color::CHANNELS);
    uint8_t *data_inerator = data;
    arr = Array2d<Color>(w, h);
    std::for_each(arr.begin(), arr.end(),
                  [&data_inerator](Array2d<Color>::element &a)
                  {
                      *a = Color(data_inerator[0], data_inerator[1], data_inerator[2], data_inerator[3]);
                      data_inerator += Color::CHANNELS;
                  });
    stbi_image_free(data);
}

Array2d<Color> &Img::operator*() { return arr; }
Array2d<Color> *Img::operator->() { return &arr; }
const Array2d<Color> &Img::operator*() const { return arr; }
const Array2d<Color> *Img::operator->() const { return &arr; }

void Img::save(std::string name)
{
    Array2d<uint8_t> out_arr(arr.get_w() * Color::CHANNELS, arr.get_h());
    std::for_each(arr.begin(), arr.end(),
                  [&out_arr](Array2d<Color>::element &a, size_t x, size_t y)
                  {
                      a->clamp();
                      out_arr(x * Color::CHANNELS + 0, y) = a->r * 255;
                      out_arr(x * Color::CHANNELS + 1, y) = a->g * 255;
                      out_arr(x * Color::CHANNELS + 2, y) = a->b * 255;
                      out_arr(x * Color::CHANNELS + 3, y) = a->a * 255;
                  });
    if (name.ends_with(".png"))
        stbi_write_png(name.c_str(), arr.get_w(), arr.get_h(), 4, out_arr.data(), out_arr.get_w() * sizeof(uint8_t));
    else if (name.ends_with(".bmp"))
        stbi_write_bmp(name.c_str(), arr.get_w(), arr.get_h(), 4, out_arr.data());
    else if (name.ends_with(".jpg"))
        stbi_write_jpg(name.c_str(), arr.get_w(), arr.get_h(), 4, out_arr.data(), 95 /*quality*/);
    else
        throw "unsupported image output format";
}
// img