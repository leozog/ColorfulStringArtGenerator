#include "string_line.h"
#include <cassert>
#include <cmath>

StringLine::StringLine(const std::vector<Vec2<double>>& nail_positions,
                       const double nail_radius,
                       const double string_radius,
                       const nail_id_t start_nail_id,
                       const Wrap start_wrap,
                       const nail_id_t end_nail_id,
                       const Wrap end_wrap)
    : start_nail_id{ start_nail_id }
    , start_wrap{ start_wrap }
    , end_nail_id{ end_nail_id }
    , end_wrap{ end_wrap }
{
    assert(start_nail_id != end_nail_id);

    Vec2<double> start_nail_pos{ nail_positions[start_nail_id] };
    Vec2<double> end_nail_pos{ nail_positions[end_nail_id] };

    double nails_dist{ start_nail_pos.dist(end_nail_pos) };
    Vec2<double> nails_dir{ (end_nail_pos - start_nail_pos).norm() };

    double nail_string_dist{ nail_radius + string_radius };

    int start_wrap_sign{ start_wrap == Wrap::CLOKWISE ? -1 : 1 };
    int end_wrap_sign{ end_wrap == Wrap::CLOKWISE ? 1 : -1 };

    if (start_wrap == end_wrap) {
        Vec2<double> nails_dir_perp{ nails_dir.perp() };
        start_pos = start_nail_pos + nails_dir_perp * start_wrap_sign * nail_string_dist;
        end_pos = end_nail_pos + nails_dir_perp * end_wrap_sign * nail_string_dist;
    } else {
        double alpha{ std::acos(nail_string_dist / (nails_dist / 2)) };
        Vec2<double> start_string_dir_perp{ nails_dir.rotate(alpha * start_wrap_sign) };
        Vec2<double> end_string_dir_perp{ nails_dir.rotate(alpha * end_wrap_sign) };
        start_pos = start_nail_pos + start_string_dir_perp * nail_string_dist;
        end_pos = end_nail_pos + end_string_dir_perp * nail_string_dist;
    }
}

nail_id_t StringLine::get_start_nail_id() const
{
    return start_nail_id;
}

StringLine::Wrap StringLine::get_start_wrap() const
{
    return start_wrap;
}

Vec2<double> StringLine::get_start_pos() const
{
    return start_pos;
}

nail_id_t StringLine::get_end_nail_id() const
{
    return end_nail_id;
}

StringLine::Wrap StringLine::get_end_wrap() const
{
    return end_wrap;
}

Vec2<double> StringLine::get_end_pos() const
{
    return end_pos;
}