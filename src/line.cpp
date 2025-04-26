#include "line.h"

#include <cmath>
#include <cstdint>
#include <functional>

// modified Gupta-Sproull algorithm
void line(double x1, double y1, double x2, double y2, double t, const std::function<void(int32_t, int32_t, double)>& f)
{
    double dx{ x2 - x1 };
    double dy{ y2 - y1 };

    // Euclidean distance between points (x1, y1) and (x2, y2)
    const double length{ sqrt((dx * dx) + (dy * dy)) };
    if (length == 0) {
        return;
    }

    if (std::abs(dx) >= std::abs(dy)) {
        // draw line from left to right

        // swap the points so the line is always drawn left to right
        if (dx < 0) {
            std::swap(x1, x2);
            std::swap(y1, y2);
            dx = -dx;
            dy = -dy;
        }
        int32_t x{ static_cast<int32_t>(std::round(x1)) };
        int32_t y{ static_cast<int32_t>(std::round(y1)) };

        const int32_t di = dy > 0 ? 1 : -1;
        dx = std::abs(dx);
        dy = std::abs(dy);
        double d = (2 * dy) - dx; // discriminator

        // Euclidean distance of point (x,y) from line (signed)
        double D = std::round(y1) - y1;

        const double sina{ dy / length };
        const double cosa{ dx / length };

        // thickness of the line
        int32_t T{ static_cast<int32_t>(std::ceil((std::fabs(t + 1) / 2.0) + std::fabs(sina))) };

        while (x <= x2) {
            for (int32_t i = -T; i <= T; i++) {
                f(x, y + i, D - (di * i * cosa));
            }
            x = x + 1;
            if (d <= 0) {
                D = D + sina;
                d = d + 2 * dy;
            } else {
                D = D + sina - cosa;
                d = d + 2 * (dy - dx);
                y = y + di;
            }
        }
    } else {
        // draw line from top to bottom

        // swap the points so the line is always drawn top to bottom
        if (dy < 0) {
            std::swap(x1, x2);
            std::swap(y1, y2);
            dx = -dx;
            dy = -dy;
        }
        int32_t x{ static_cast<int32_t>(std::round(x1)) };
        int32_t y{ static_cast<int32_t>(std::round(y1)) };

        const int32_t di = dx > 0 ? 1 : -1;
        dx = std::abs(dx);
        dy = std::abs(dy);
        double d = (2 * dx) - dy; // discriminator

        // Euclidean distance of point (x,y) from line (signed)
        double D = std::round(x1) - x1;

        const double sina{ dy / length };
        const double cosa{ dx / length };

        // thickness of the line
        int32_t T{ static_cast<int32_t>(std::ceil((std::fabs(t) / 2.0) + std::fabs(cosa))) };

        while (y <= y2) {
            for (int32_t i = -T; i <= T; i++) {
                f(x + i, y, D - (di * i * sina));
            }
            y = y + 1;
            if (d <= 0) {
                D = D + cosa;
                d = d + 2 * dx;
            } else {
                D = D + cosa - sina;
                d = d + 2 * (dx - dy);
                x = x + di;
            }
        }
    }
}

void line(const Vec2<double>& p1,
          const Vec2<double>& p2,
          double t,
          const std::function<void(int32_t, int32_t, double)>& f)
{
    line(p1[0], p1[1], p2[0], p2[1], t, f);
}