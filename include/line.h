#pragma once
#include <cstdint>
#include <functional>

// modified Gupta-Sproull algorithm
void line(int32_t x1,
          int32_t x2,
          int32_t y1,
          int32_t y2,
          double t,
          const std::function<void(int32_t, int32_t, double)>& f);