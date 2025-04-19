#include "img.h"
#include "array2d.h"
#include <cstddef>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include <algorithm>

// img
Img::Img(size_t w, size_t h, Color color)
    : Array2d<Color>{ w, h }
{
    if (color != Color(0.0, 0.0, 0.0, 0.0)) {
        std::fill(begin(), end(), color);
    }
}

Img Img::load(const std::string& path)
{
    int w, h, n;
    uint8_t* data = stbi_load(path.c_str(), &w, &h, &n, Color::CHANNELS);
    if (data == nullptr) {
        throw "failed to load image";
    }
    uint8_t* data_inerator = data;
    Img img{ static_cast<size_t>(w), static_cast<size_t>(h) };
    std::for_each(img.begin(), img.end(), [&data_inerator](auto a) {
        *a = Color{ data_inerator[0], data_inerator[1], data_inerator[2], data_inerator[3] };
        data_inerator += Color::CHANNELS;
    });
    stbi_image_free(data);
    return img;
}

void Img::save(const std::string& path)
{
    static_assert(Color::CHANNELS == 4);
    Array2d<uint32_t> out_arr(get_w(), get_h());
    std::transform(cbegin(), cend(), out_arr.begin(), [](const auto& c) {
        auto clamped = c->clamp();
        return static_cast<uint32_t>(clamped.r() * 255) | static_cast<uint32_t>(clamped.g() * 255) << 8 |
               static_cast<uint32_t>(clamped.b() * 255) << 16 | static_cast<uint32_t>(clamped.a() * 255) << 24;
    });
    int success = 0;
    if (path.ends_with(".png")) {
        success = stbi_write_png(path.c_str(),
                                 static_cast<int>(get_w()),
                                 static_cast<int>(get_h()),
                                 Color::CHANNELS,
                                 out_arr.data(),
                                 static_cast<int>(out_arr.get_w() * sizeof(uint32_t)));
    } else if (path.ends_with(".bmp")) {
        success = stbi_write_bmp(path.c_str(),
                                 static_cast<int>(get_w()), //
                                 static_cast<int>(get_h()),
                                 Color::CHANNELS,
                                 out_arr.data());
    } else if (path.ends_with(".jpg")) {
        success = stbi_write_jpg(path.c_str(),
                                 static_cast<int>(get_w()),
                                 static_cast<int>(get_h()),
                                 Color::CHANNELS,
                                 out_arr.data(),
                                 95 /*quality*/);
    } else {
        throw "unsupported image output format";
    }
    if (success == 0) {
        throw "failed to save image";
    }
}

Img Img::operator+(const Img& other) const // nw czy to dobrze dziala. sprawdz doc transform
{
    Img output{ get_w(), get_h() };
    std::transform(cbegin(), cend(), other.cbegin(), output.begin(), [](const auto a, const auto b) {
        return a->clamp() + b->clamp();
    });
    return output;
}

Img Img::operator+(const Color& color) const
{
    Img output{ get_w(), get_h() };
    std::transform(cbegin(), cend(), output.begin(), [&color](const auto a) { return *a + color; });
    return output;
}

Img& Img::operator+=(const Img& other)
{
    std::transform(cbegin(), cend(), other.cbegin(), begin(), [](const auto a, const auto b) { return *a + *b; });
    return *this;
}

Img& Img::operator+=(const Color& color)
{
    std::for_each(begin(), end(), [&color](auto a) { *a += color; });
    return *this;
}