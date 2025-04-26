#pragma once
#include "color.h"
#include "img.h"
#include "thread_pool.h"

#include <cstdint>
#include <optional>
#include <unordered_map>
#include <vector>

class ImageColorQuantizer
{
    std::unordered_map<Color, uint32_t> colors;
    const std::vector<Color> const_centroids;
    ThreadPool& thread_pool;

public:
    ImageColorQuantizer(const Img& img,
                        const std::vector<Color>& const_centroids,
                        ThreadPool& thread_pool,
                        const std::optional<Array2d<uint8_t>>& mask = std::nullopt);

    [[nodiscard]] std::vector<Color> get_pallete(uint32_t n_colors,
                                                 uint32_t n_iter,
                                                 double threshold,
                                                 size_t seed = 0) const;

private:
    std::pair<std::vector<Color>, double> k_means(size_t k, double threshold) const;
    static Color random_color();
};
