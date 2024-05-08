#include "img.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

// color
color::color(int r, int g, int b, int a) : r{r / 255.f}, g{g / 255.f}, b{b / 255.f}, a{a / 255.f} {}

color::color(double r, double g, double b, double a) : r{static_cast<float>(r)}, g{static_cast<float>(g)}, b{static_cast<float>(b)}, a{static_cast<float>(a)} {}

color color::operator+(const color &x) const
{
    float ao = x.a + a * (1. - x.a);
    return color(
        (x.r * x.a + r * a * (1 - x.a)) / ao,
        (x.g * x.a + g * a * (1 - x.a)) / ao,
        (x.b * x.a + b * a * (1 - x.a)) / ao,
        ao);
}

color color::operator*(const double x) const
{
    return color(
        r * x,
        g * x,
        b * x,
        a);
}

color &color::operator+=(const color &x)
{
    *this = *this + x;
    return *this;
}

color &color::operator*=(const double x)
{
    *this = *this * x;
    return *this;
}

color color::clamp() const
{
    return color(
        std::clamp(r, 0.f, 1.f),
        std::clamp(g, 0.f, 1.f),
        std::clamp(b, 0.f, 1.f),
        std::clamp(a, 0.f, 1.f));
}
// color

// img
img::img(size_t w, size_t h) : arr(w, h)
{
}

img::img(std::string name) : arr(0, 0)
{
    int w, h, n;
    uint8_t *data = stbi_load(name.c_str(), &w, &h, &n, color::CHANNELS);
    arr = array2d<color>(w, h);
    arr.for_each([data](color &a, size_t i)
                 { a = color(data[color::CHANNELS * i + 0],
                             data[color::CHANNELS * i + 1],
                             data[color::CHANNELS * i + 2],
                             data[color::CHANNELS * i + 3]); });
    stbi_image_free(data);
}

array2d<color> &img::operator*() { return arr; }
array2d<color> *img::operator->() { return &arr; }

void img::save(std::string name)
{
    array2d<uint8_t> out_arr(arr.get_w() * color::CHANNELS, arr.get_h());
    arr.for_each(
        [&out_arr](color &a, size_t x, size_t y)
        {
            a.clamp();
            out_arr(x * color::CHANNELS + 0, y) = a.r * 255;
            out_arr(x * color::CHANNELS + 1, y) = a.g * 255;
            out_arr(x * color::CHANNELS + 2, y) = a.b * 255;
            out_arr(x * color::CHANNELS + 3, y) = a.a * 255;
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