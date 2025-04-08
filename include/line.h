#pragma once
#include <cstdint>
#include <functional>

// modified Gupta-Sproull algorithm
void line(double x1, double x2, double y1, double y2, double t, const std::function<void(int32_t, int32_t, double)>& f);