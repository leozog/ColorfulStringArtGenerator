#pragma once
#include "vec.h"

#include <cstdint>
#include <functional>

// modified Gupta-Sproull algorithm
void line(double x1, double y1, double x2, double y2, double t, const std::function<void(int32_t, int32_t, double)>& f);
void line(const Vec2<double>& p1,
          const Vec2<double>& p2,
          double t,
          const std::function<void(int32_t, int32_t, double)>& f);