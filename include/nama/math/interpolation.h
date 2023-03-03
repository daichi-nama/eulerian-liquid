#pragma once

#include <cassert>
#include <cmath>
#include <type_traits>
#include <utility>

namespace nama::math {

template <typename U>
inline std::pair<int, U> calc_index_and_fraction(U pos, U origin, U dx) {
    U pos_transformed = (pos - origin - static_cast<U>(0.5) * dx) / dx;
    int index         = std::floor(pos_transformed);
    return {index, pos_transformed - static_cast<U>(index)};
}

template <typename T, typename U>
inline T lerp(const T v[2], const U f[1]) {
    assert(f[0] >= 0 && f[0] <= 1);
    return (1 - f[0]) * v[0] + f[0] * v[1];
}

template <typename T, typename U>
inline T bilerp(const T v[4], const U f[2]) {
    T w[2] = {lerp(v, f), lerp(v + 2, f)};
    return lerp(w, f + 1);
}

template <typename T, typename U>
inline T trilerp(const T v[8], const U f[3]) {
    T w[2] = {bilerp(v, f), bilerp(v + 4, f)};
    return lerp(w, f + 2);
}

template <typename UV, typename Grid>
inline auto lerp(const UV& x, const Grid& grid) -> typename Grid::value_type {
    static_assert(Grid::dim == 1 || Grid::dim == 2 || Grid::dim == 3, "invalid dimension");
    using T = typename Grid::value_type;
    using U = typename Grid::coordinate_type;
    if constexpr (Grid::dim == 1) {
        auto [i, fx] = calc_index_and_fraction(x, grid.origin(), grid.dx());
        T v[2]       = {
            grid.extrapolate(i),
            grid.extrapolate(i + 1),
        };
        U f[1] = {fx};
        return lerp(v, f);
    } else if constexpr (Grid::dim == 2) {
        auto [i, fx] = calc_index_and_fraction(x[0], grid.origin()[0], grid.dx());
        auto [j, fy] = calc_index_and_fraction(x[1], grid.origin()[1], grid.dx());
        T v[4]       = {
            grid.extrapolate(i, j),
            grid.extrapolate(i + 1, j),
            grid.extrapolate(i, j + 1),
            grid.extrapolate(i + 1, j + 1),
        };
        U f[2] = {fx, fy};
        return bilerp(v, f);
    } else {
        auto [i, fx] = calc_index_and_fraction(x[0], grid.origin()[0], grid.dx());
        auto [j, fy] = calc_index_and_fraction(x[1], grid.origin()[1], grid.dx());
        auto [k, fz] = calc_index_and_fraction(x[2], grid.origin()[2], grid.dx());
        T v[8]       = {
            grid.extrapolate(i, j, k),         grid.extrapolate(i + 1, j, k),
            grid.extrapolate(i, j + 1, k),     grid.extrapolate(i + 1, j + 1, k),
            grid.extrapolate(i, j, k + 1),     grid.extrapolate(i + 1, j, k + 1),
            grid.extrapolate(i, j + 1, k + 1), grid.extrapolate(i + 1, j + 1, k + 1),
        };
        U f[3] = {fx, fy, fz};
        return trilerp(v, f);
    }
}

template <typename T, typename U>
inline T weno6_interp_1d(const T v[6], const U f[1]) {
    static T eps = 1e-6;
    T x_array[6];
    for (int i = 0; i < 6; ++i) {
        x_array[i] = -2.0 + i;
    }
    T x = x_array[2] + f[0];
    T p[3], C[3], IS[3], w[3];
    for (int i = 0; i < 3; ++i) {
        p[i] = v[i] + (v[1 + i] - v[i]) * (x - x_array[i])
               + (v[2 + i] - 2 * v[1 + i] + v[i]) * 0.5 * (x - x_array[i]) * (x - x_array[1 + i])
               + (v[3 + i] - 3 * v[2 + i] + 3 * v[1 + i] - v[i]) / 6.0 * (x - x_array[i])
                     * (x - x_array[1 + i]) * (x - x_array[2 + i]);
    }
    C[0]  = (x_array[4] - x) * (x_array[5] - x) / 20.0;
    C[1]  = (x_array[5] - x) * (x - x_array[0]) / 10.0;
    C[2]  = (x - x_array[0]) * (x - x_array[1]) / 20.0;
    IS[0] = (814 * v[3] * v[3] + 4326 * v[2] * v[2] + 2976 * v[1] * v[1] + 244 * v[0] * v[0]
             - 3579 * v[2] * v[3] - 6927 * v[2] * v[1] + 1854 * v[2] * v[0] + 2634 * v[3] * v[1]
             - 683 * v[3] * v[0] - 1659 * v[1] * v[0])
            / 180.0;
    IS[1] = (1986 * v[3] * v[3] + 1986 * v[2] * v[2] + 244 * v[1] * v[1] + 244 * v[4] * v[4]
             + 1074 * v[2] * v[4] - 3777 * v[2] * v[3] - 1269 * v[2] * v[1] + 1074 * v[3] * v[1]
             - 1269 * v[4] * v[3] - 293 * v[4] * v[1])
            / 180.0;
    IS[2] = (814 * v[2] * v[2] + 4326 * v[3] * v[3] + 2976 * v[4] * v[4] + 244 * v[5] * v[5]
             - 683 * v[2] * v[5] + 2634 * v[2] * v[4] - 3579 * v[2] * v[3] - 6927 * v[3] * v[4]
             + 1854 * v[3] * v[5] - 1659 * v[4] * v[5])
            / 180.0;
    for (int i = 0; i < 3; ++i) {
        C[i] /= (eps + IS[i]) * (eps + IS[i]);
    }
    for (int i = 0; i < 3; ++i) {
        w[i] = C[i] / (C[0] + C[1] + C[2]);
    }
    return w[0] * p[0] + w[1] * p[1] + w[2] * p[2];
}

template <typename T, typename U>
inline T weno6_interp_2d(const T v[36], const U f[2]) {
    T vy[6];
    for (int i = 0; i < 6; ++i) {
        T v_sub[6]
            = {v[i * 6], v[i * 6 + 1], v[i * 6 + 2], v[i * 6 + 3], v[i * 6 + 4], v[i * 6 + 5]};
        vy[i] = weno6_interp_1d(v_sub, f);
    }
    return weno6_interp_1d(vy, f + 1);
}

template <typename T, typename U>
inline T weno6_interp_3d(const T v[216], const U f[3]) {
    T vz[6];
    for (int j = 0; j < 6; ++j) {
        T vy[6];
        for (int i = 0; i < 6; ++i) {
            T v_sub[6] = {v[i * 6 + j * 36],     v[i * 6 + 1 + j * 36], v[i * 6 + 2 + j * 36],
                          v[i * 6 + 3 + j * 36], v[i * 6 + 4 + j * 36], v[i * 6 + 5 + j * 36]};
            vy[i]      = weno6_interp_1d(v_sub, f);
        }
        vz[j] = weno6_interp_1d(vy, f + 1);
    }
    return weno6_interp_1d(vz, f + 2);
}

template <typename UV, typename Grid>
inline auto weno6_interp(const UV& x, const Grid& grid) -> typename Grid::value_type {
    static_assert(Grid::dim == 1 || Grid::dim == 2 || Grid::dim == 3, "invalid dimension");
    using T = typename Grid::value_type;
    using U = typename Grid::coordinate_type;
    if constexpr (Grid::dim == 1) {
        auto [i, fx] = calc_index_and_fraction(x, grid.origin(), grid.dx());
        T v[6];
        for (int di = 0; di < 6; ++di) {
            int ii = i + di - 2;
            v[di]  = grid.extrapolate(ii);
        }
        U f[1] = {fx};
        return weno6_interp_1d(v, f);
    } else if constexpr (Grid::dim == 2) {
        auto [i, fx] = calc_index_and_fraction(x[0], grid.origin()[0], grid.dx());
        auto [j, fy] = calc_index_and_fraction(x[1], grid.origin()[1], grid.dx());
        T v[36];
        for (int dj = 0; dj < 6; ++dj) {
            for (int di = 0; di < 6; ++di) {
                int ii         = i + di - 2;
                int jj         = j + dj - 2;
                v[di + dj * 6] = grid.extrapolate(ii, jj);
            }
        }
        U f[2] = {fx, fy};
        return weno6_interp_2d(v, f);
    } else {
        auto [i, fx] = calc_index_and_fraction(x[0], grid.origin()[0], grid.dx());
        auto [j, fy] = calc_index_and_fraction(x[1], grid.origin()[1], grid.dx());
        auto [k, fz] = calc_index_and_fraction(x[2], grid.origin()[2], grid.dx());
        T v[216];
        for (int dk = 0; dk < 6; ++dk) {
            for (int dj = 0; dj < 6; ++dj) {
                for (int di = 0; di < 6; ++di) {
                    int ii                      = i + di - 2;
                    int jj                      = j + dj - 2;
                    int kk                      = k + dk - 2;
                    v[di + dj * 6 + dk * 6 * 6] = grid.extrapolate(ii, jj, kk);
                }
            }
        }
        U f[3] = {fx, fy, fz};
        return weno6_interp_3d(v, f);
    }
}

template <typename TV, typename UV, typename StaggeredGrid>
inline TV lerp_staggered(const UV& x, const StaggeredGrid& grid) {
    TV result;
    for (int i = 0; i < StaggeredGrid::dim; ++i) {
        result[i] = lerp(x, grid[i]);
    }
    return result;
}

template <typename TV, typename UV, typename StaggeredGrid>
inline TV weno6_interp_staggered(const UV& x, const StaggeredGrid& grid) {
    TV result;
    for (int i = 0; i < StaggeredGrid::dim; ++i) {
        result[i] = weno6_interp(x, grid[i]);
    }
    return result;
}

}  // namespace nama::math
