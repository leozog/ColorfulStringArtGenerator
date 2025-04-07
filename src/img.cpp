#include "img.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include <algorithm>

// color
Color::Color(double r, double g, double b, double a)
    : Vec(r, g, b, a)
{
}

Color::Color(int r, int g, int b, int a)
    : Color(r / 255.f, g / 255.f, b / 255.f, a / 255.f)
{
}

Color Color::operator+(const Color& x) const
{
    float ao = x.a() + a() * (1. - x.a());
    return Color((x.r() * x.a() + r() * a() * (1 - x.a())) / ao,
                 (x.g() * x.a() + g() * a() * (1 - x.a())) / ao,
                 (x.b() * x.a() + b() * a() * (1 - x.a())) / ao,
                 ao);
}

Color Color::operator-(const Color& x) const
{
    return this->operator+(Color(-x.r(), -x.g(), -x.b(), -x.a()));
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
    return Color(
        std::clamp(r(), 0.f, 1.f), std::clamp(g(), 0.f, 1.f), std::clamp(b(), 0.f, 1.f), std::clamp(a(), 0.f, 1.f));
}
// color

// img
Img::Img(size_t w, size_t h)
    : arr(w, h)
{
}

Img::Img(std::string path)
{
    int w, h, n;
    uint8_t* data = stbi_load(path.c_str(), &w, &h, &n, Color::CHANNELS);
    uint8_t* data_inerator = data;
    arr = Array2d<Color>(w, h);
    std::for_each(arr.begin(), arr.end(), [&data_inerator](Array2d<Color>::Element a) {
        *a = Color(data_inerator[0], data_inerator[1], data_inerator[2], data_inerator[3]);
        data_inerator += Color::CHANNELS;
    });
    stbi_image_free(data);
}

Array2d<Color>& Img::operator*()
{
    return arr;
}

Array2d<Color>* Img::operator->()
{
    return &arr;
}

const Array2d<Color>& Img::operator*() const
{
    return arr;
}

const Array2d<Color>* Img::operator->() const
{
    return &arr;
}

void Img::save(std::string path)
{
    Array2d<uint8_t> out_arr(arr.get_w() * Color::CHANNELS, arr.get_h());
    Array2d<uint8_t>::Iterator out_it = out_arr.begin();
    for (Array2d<Color>::Element it : arr) {
        it = it->clamp();
        *out_it++ = static_cast<uint8_t>(it->r() * 255);
        *out_it++ = static_cast<uint8_t>(it->g() * 255);
        *out_it++ = static_cast<uint8_t>(it->b() * 255);
        *out_it++ = static_cast<uint8_t>(it->a() * 255);
    }
    if (path.ends_with(".png")) {
        stbi_write_png(path.c_str(), arr.get_w(), arr.get_h(), 4, out_arr.data(), out_arr.get_w() * sizeof(uint8_t));
    } else if (path.ends_with(".bmp")) {
        stbi_write_bmp(path.c_str(), arr.get_w(), arr.get_h(), 4, out_arr.data());
    } else if (path.ends_with(".jpg")) {
        stbi_write_jpg(path.c_str(), arr.get_w(), arr.get_h(), 4, out_arr.data(), 95 /*quality*/);
    } else {
        throw "unsupported image output format";
    }
}
// img