#include "line.h"

#include <cmath>
#include <stdint.h>
#include <functional>

// modified Gupta-Sproull algorithm
void line(int32_t x1, int32_t x2, int32_t y1, int32_t y2, double t, std::function<void(int32_t, int32_t, double)> f)
{
    int32_t dx = x2 - x1;
    int32_t dy = y2 - y1;

    // Euclidean distance of point (x,y) from line (signed)
    double D = 0;

    // Euclidean distance between points (x1, y1) and (x2, y2)
    double length = sqrt(dx * dx + dy * dy);
    if (length == 0)
        return;

    if (std::abs(dx) >= std::abs(dy))
    {
        // draw line from left to right

        // swap the points so the line is always drawn left to right
        if (dx < 0)
        {
            std::swap(x1, x2);
            std::swap(y1, y2);
            dx = -dx;
            dy = -dy;
        }
        int32_t x = x1;
        int32_t y = y1;

        int32_t di = dy > 0 ? 1 : -1;
        dx = std::abs(dx);
        dy = std::abs(dy);
        int32_t d = 2 * dy - dx; // discriminator

        double sina = (double)dy / length;
        double cosa = (double)dx / length;

        // thickness of the line
        int32_t T = std::ceil(std::fabs(t - 1) / 2 + std::fabs(sina));

        while (x <= x2)
        {
            for (int32_t i = -T; i <= T; i++)
            {
                f(x, y + i, D - di * i * cosa);
            }
            x = x + 1;
            if (d <= 0)
            {
                D = D + sina;
                d = d + 2 * dy;
            }
            else
            {
                D = D + sina - cosa;
                d = d + 2 * (dy - dx);
                y = y + di;
            }
        }
    }
    else
    {
        // draw line from top to bottom

        // swap the points so the line is always drawn top to bottom
        if (dy < 0)
        {
            std::swap(x1, x2);
            std::swap(y1, y2);
            dx = -dx;
            dy = -dy;
        }
        int32_t x = x1;
        int32_t y = y1;

        int32_t di = dx >= 0 ? 1 : -1;
        dx = std::abs(dx);
        dy = std::abs(dy);
        int32_t d = 2 * dx - dy; // discriminator

        double sina = (double)dy / length;
        double cosa = (double)dx / length;

        // thickness of the line
        int32_t T = std::ceil(std::fabs(t - 1) / 2 + cosa);

        while (y <= y2)
        {
            for (int32_t i = -T; i <= T; i++)
            {
                f(x + i, y, D - di * i * sina);
            }
            y = y + 1;
            if (d <= 0)
            {
                D = D + cosa;
                d = d + 2 * dx;
            }
            else
            {
                D = D + cosa - sina;
                d = d + 2 * (dx - dy);
                x = x + di;
            }
        }
    }
}