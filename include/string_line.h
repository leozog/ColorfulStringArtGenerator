#pragma once
#include "vec.h"
#include <cstdint>
#include <vector>

using nail_id_t = uint32_t;

class StringLine
{
public:
    enum class Wrap : uint8_t
    {
        CLOKWISE,
        ANTICLOCKWISE
    };

private:
    const nail_id_t start_nail_id;
    const Wrap start_wrap;
    mutable Vec2<double> start_pos;
    const nail_id_t end_nail_id;
    const Wrap end_wrap;
    mutable Vec2<double> end_pos;

public:
    StringLine(const std::vector<Vec2<double>>& nail_positions,
               const double nail_radius,
               const double string_radius,
               const nail_id_t start_nail_id,
               const Wrap start_wrap,
               const nail_id_t end_nail_id,
               const Wrap end_wrap);

    [[nodiscard]] nail_id_t get_start_nail_id() const;
    [[nodiscard]] Wrap get_start_wrap() const;
    [[nodiscard]] Vec2<double> get_start_pos() const;
    [[nodiscard]] nail_id_t get_end_nail_id() const;
    [[nodiscard]] Wrap get_end_wrap() const;
    [[nodiscard]] Vec2<double> get_end_pos() const;
};
