#include "img_color_quantizer.h"
#include "array2d.h"
#include "color.h"
#include "logger.h"
#include "thread_pool.h"
#include "thread_rng.h"
#include "vec.h"

ImageColorQuantizer::ImageColorQuantizer(const Img& img,
                                         const std::vector<Color>& const_centroids,
                                         ThreadPool& thread_pool,
                                         const std::optional<Array2d<uint8_t>>& mask)
    : const_centroids{ const_centroids }
    , thread_pool{ thread_pool }
{
    if (mask) {
        assert(img.get_w() == mask->get_w());
        assert(img.get_h() == mask->get_h());
    }

    const uint32_t n_tasks{ thread_pool.get_n_threads() };
    const size_t rows_per_task{ img.get_h() / n_tasks > 0 ? img.get_h() / n_tasks : 1 };

    std::function<std::unordered_map<Color, size_t>(Img::ConstRegion)> f = [&](Img::ConstRegion region) {
        std::unordered_map<Color, size_t> task_colors;
        task_colors.reserve(rows_per_task * img.get_w());
        std::for_each(region.cbegin(), region.cend(), [&](const auto& color) {
            if (!mask || (*mask)(color.get_x(), color.get_y())) {
                task_colors[*color]++;
            }
        });
        return task_colors;
    };

    std::vector<std::future<std::unordered_map<Color, size_t>>> futures;
    futures.reserve(n_tasks);
    for (size_t y_start{ 0 }; y_start < img.get_h(); y_start += rows_per_task) {
        size_t y_end{ std::min(y_start + rows_per_task, img.get_h()) };
        futures.push_back(thread_pool.submit(1, f, img.get_cregion(0, y_start, img.get_w(), y_end)));
    }

    colors.reserve(img.get_w() * img.get_h());
    for (auto& f : futures) {
        auto task_colors{ f.get() };
        for (const auto& [color, count] : task_colors) {
            colors[color] += count;
        }
    }
}

std::vector<Color> ImageColorQuantizer::get_pallete(uint32_t n_colors,
                                                    uint32_t n_iter,
                                                    double threshold,
                                                    size_t seed) const
{
    if (n_colors == 0 || colors.empty()) {
        Logger::error("No colors to create palette from");
        return {};
    }
    if (n_colors > colors.size()) {
        Logger::warn("Requested {}-color palette, but only {} colors available", n_colors, colors.size());
        n_colors = static_cast<uint32_t>(colors.size());
    }
    if (n_colors == colors.size()) {
        std::vector<Color> palette;
        palette.reserve(colors.size());
        std::transform(colors.cbegin(), colors.cend(), std::back_inserter(palette), [](const auto& color_pair) {
            return color_pair.first;
        });
        return palette;
    }
    Logger::info("Clustering {} colors into {} colors", colors.size(), n_colors);

    std::function<std::pair<std::vector<Color>, double>(uint32_t, double)> f =
        [this](uint32_t n_colors, double threshold) { return k_means(n_colors, threshold); };

    std::vector<std::future<std::pair<std::vector<Color>, double>>> futures;
    futures.reserve(n_iter);
    for (uint32_t i = 0; i < n_iter; ++i) {
        futures.push_back(thread_pool.submit(1, f, n_colors, threshold));
    }

    std::vector<std::pair<std::vector<Color>, double>> results;
    results.reserve(n_iter);
    for (auto& f : futures) {
        results.emplace_back(f.get());
    }

    const auto best_result = std::min_element(
        results.cbegin(), results.cend(), [](const auto& a, const auto& b) { return a.second < b.second; });
    const auto& [centroids, inertia] = *best_result;

    return centroids;
}

std::pair<std::vector<Color>, double> ImageColorQuantizer::k_means(size_t k, double threshold) const
{
    std::vector<Color> centroids;
    centroids.reserve(k);

    std::vector<std::pair<Vec4<size_t>, size_t>> new_centroids;
    new_centroids.reserve(k);

    for (uint32_t i = 0; i < k; ++i) {
        centroids.push_back(random_color());
    }

    double delta{ std::numeric_limits<double>::max() };

    while (delta > threshold) {
        new_centroids.assign(k, { { 0, 0, 0, 0 }, 0 });

        for (const auto& [color, color_count] : colors) {
            const auto closest_const_centroid = std::min_element(
                const_centroids.begin(), const_centroids.end(), [&color](const Color& a, const Color& b) {
                    return color.dist_sq(a) < color.dist_sq(b);
                });
            const auto closest_centroid =
                std::min_element(centroids.begin(), centroids.end(), [&color](const Color& a, const Color& b) {
                    return color.dist_sq(a) < color.dist_sq(b);
                });

            if (closest_const_centroid != const_centroids.end() &&
                color.dist_sq(*closest_const_centroid) < color.dist_sq(*closest_centroid)) {
                continue;
            }

            const auto index = std::distance(centroids.begin(), closest_centroid);

            auto& [new_centroid, new_centroid_count] = new_centroids[index];
            new_centroid += static_cast<Vec4<size_t>>(color * 255) * color_count;
            new_centroid_count += color_count;
        }

        delta = 0.0;
        std::transform(
            new_centroids.cbegin(),
            new_centroids.cend(),
            centroids.cbegin(),
            centroids.begin(),
            [&delta](const auto& new_centroid_pair, const Color& centroid) -> Color {
                const auto& [new_centroid, new_centroid_count] = new_centroid_pair;
                if (new_centroid_count == 0) {
                    return centroid;
                }
                Vec4<float> new_centroid_float{ new_centroid };
                new_centroid_float /= new_centroid_count * 255.0f;
                delta += new_centroid_float.dist_sq(centroid);
                return { new_centroid_float[0], new_centroid_float[1], new_centroid_float[2], new_centroid_float[3] };
            });
    }

    double inertia{ 0.0 };
    for (const auto& [color, color_count] : colors) {
        const auto closest_const_centroid =
            std::min_element(const_centroids.begin(), const_centroids.end(), [&color](const Color& a, const Color& b) {
                return color.dist_sq(a) < color.dist_sq(b);
            });
        const auto closest_centroid =
            std::min_element(centroids.begin(), centroids.end(), [&color](const Color& a, const Color& b) {
                return color.dist_sq(a) < color.dist_sq(b);
            });

        if (closest_const_centroid != const_centroids.end() &&
            color.dist_sq(*closest_const_centroid) < color.dist_sq(*closest_centroid)) {
            inertia += color.dist_sq(*closest_const_centroid) * color_count;
        } else {
            inertia += color.dist_sq(*closest_centroid) * color_count;
        }
    }

    return { centroids, inertia };
}

Color ImageColorQuantizer::random_color()
{
    return Color{ ThreadRng::uniform_int(0, 255), ThreadRng::uniform_int(0, 255), ThreadRng::uniform_int(0, 255) };
}