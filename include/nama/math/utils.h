#pragma once

#include <cmath>

namespace nama::math {

template <typename T>
inline T min_max_normalization(T val, T min, T max) {
    return (val - min) / (max - min);
}

}  // namespace nama::math
