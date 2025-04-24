#pragma once
#include "color.h"
#include "img.h"
#include "thread_pool.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

class ImageColorQuantizer
{
    std::unordered_map<Color, uint32_t> colors;
    ThreadPool& thread_pool;

public:
    ImageColorQuantizer(const Img& img, const Array2d<uint8_t>& mask, ThreadPool& thread_pool);
    ImageColorQuantizer(const Img& img, ThreadPool& thread_pool);
    [[nodiscard]] std::vector<Color> get_pallete(uint32_t n_colors,
                                                 uint32_t n_iter,
                                                 double threshold,
                                                 size_t seed = 0) const;

private:
    std::pair<std::vector<Color>, double> k_means(size_t k, double threshold) const;
    static Color random_color();
};
