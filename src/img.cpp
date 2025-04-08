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
    : Color{ static_cast<float>(r) / 255.f,
             static_cast<float>(g) / 255.f,
             static_cast<float>(b) / 255.f,
             static_cast<float>(a) / 255.f }
{
}

Color Color::operator+(const Color& x) const
{
    double ao = x.a() + a() * (1. - x.a());
    return { (x.r() * x.a() + r() * a() * (1 - x.a())) / ao,
             (x.g() * x.a() + g() * a() * (1 - x.a())) / ao,
             (x.b() * x.a() + b() * a() * (1 - x.a())) / ao,
             ao };
}

Color Color::operator-(const Color& x) const
{
    return this->operator+(Color{ -x.r(), -x.g(), -x.b(), -x.a() });
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
// color

// img
Img::Img(size_t w, size_t h)
    : arr(w, h)
{
}

Img::Img(const std::string& path)
{
    int w, h, n;
    uint8_t* data = stbi_load(path.c_str(), &w, &h, &n, Color::CHANNELS);
    uint8_t* data_inerator = data;
    arr = Array2d<Color>(w, h);
    std::for_each(arr.begin(), arr.end(), [&data_inerator](auto a) {
        *a = Color{ static_cast<uint8_t>(data_inerator[0]),
                    static_cast<uint8_t>(data_inerator[1]),
                    static_cast<uint8_t>(data_inerator[2]),
                    static_cast<uint8_t>(data_inerator[3]) };
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

void Img::save(const std::string& path)
{
    static_assert(Color::CHANNELS == 4);
    Array2d<uint32_t> out_arr(arr.get_w(), arr.get_h());
    std::transform(arr.cbegin(), arr.cend(), out_arr.begin(), [](const auto& c) {
        auto clamped = c->clamp();
        return static_cast<uint32_t>(clamped.r() * 255) | static_cast<uint32_t>(clamped.g() * 255) << 8 |
               static_cast<uint32_t>(clamped.b() * 255) << 16 | static_cast<uint32_t>(clamped.a() * 255) << 24;
    });
    if (path.ends_with(".png")) {
        stbi_write_png(path.c_str(),
                       static_cast<int>(arr.get_w()),
                       static_cast<int>(arr.get_h()),
                       Color::CHANNELS,
                       out_arr.data(),
                       static_cast<int>(out_arr.get_w() * sizeof(uint32_t)));
    } else if (path.ends_with(".bmp")) {
        stbi_write_bmp(path.c_str(),
                       static_cast<int>(arr.get_w()), //
                       static_cast<int>(arr.get_h()),
                       Color::CHANNELS,
                       out_arr.data());
    } else if (path.ends_with(".jpg")) {
        stbi_write_jpg(path.c_str(),
                       static_cast<int>(arr.get_w()),
                       static_cast<int>(arr.get_h()),
                       Color::CHANNELS,
                       out_arr.data(),
                       95 /*quality*/);
    } else {
        throw "unsupported image output format";
    }
}
// img