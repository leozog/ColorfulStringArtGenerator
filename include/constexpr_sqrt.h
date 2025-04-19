#include <cmath>
#include <limits>

template<typename T>
constexpr T constexpr_sqrt(T x) noexcept // TODO: replace with std::sqrt when C++26 drops
{
    if consteval {
        if (x >= 0 && x < std::numeric_limits<double>::infinity()) {
            double curr{ x };
            double prev{ 0 };
            while (curr != prev) {
                prev = curr;
                curr = (curr + x / curr) / 2.0;
            }
            return static_cast<T>(curr);
        }
        return std::numeric_limits<double>::quiet_NaN();
    } else {
        return std::sqrt(x);
    }
}