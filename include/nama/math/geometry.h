#pragma once

namespace nama::math {

template <typename TV>
TV calc_normal(const TV& p0, const TV& p1, const TV& p2) {
    return (p1 - p0).cross(p2 - p0).normalized();
}

template <typename TV>
TV calc_normal(const TV& p0, const TV& p1, const TV& p2, [[maybe_unused]] const TV& p3) {
    return calc_normal(p0, p1, p2);
}

}  // namespace nama::math
