#pragma once
#include <cmath>
#include <stdint.h>
#include <functional>

// https://algo2.iti.kit.edu/wassenberg/LineAA/LineAA.pdf
void line_chen(int32_t x1, int32_t x2, int32_t y1, int32_t y2, std::function<void(int32_t, int32_t, double)> f)
{

    int32_t dx = x2 - x1;
    int32_t dy = y2 - y1;
    int32_t d = 2 * dy - dx; // discriminator

    // Euclidean distance of point (x,y) from line
    double D = 0;

    // Euclidean distance between points (x1, y1) and (x2, y2)
    double length = std::sqrt(dx * dx + dy * dy);

    double sina = dy / length;
    double cosa = dx / length;

    if (std::abs(dx) > std::abs(dy))
    {
        if (x1 > x2)
        {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }
        int32_t x = x1;
        int32_t y = y1;

        while (x <= x2)
        {
            f(x, y - 1, D + cosa);
            f(x, y, D);
            f(x, y + 1, D - cosa);
            x = x + 1;
            if (d <= 0)
            {
                D = D + sina;
                d = d + 2. * dy;
            }
            else
            {
                D = D + sina - cosa;
                d = d + 2. * (dy - dx);
                y = y + 1;
            }
        }
    }
    else
    {
        if (y1 > y2)
        {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }
        int32_t x = x1;
        int32_t y = y1;

        while (y <= y2)
        {
            f(x - 1, y, D + cosa);
            f(x, y, D);
            f(x + 1, y, D - cosa);
            y = y + 1;
            if (d <= 0)
            {
                D = D + sina;
                d = d + 2. * dx;
            }
            else
            {
                D = D + sina - cosa;
                d = d + 2. * (dy - dx);
                x = x + 1;
            }
        }
    }
}