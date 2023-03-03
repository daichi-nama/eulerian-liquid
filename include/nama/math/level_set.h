#pragma once

#include <nama/common/grid.h>
#include <nama/common/meshing.h>

#include <algorithm>
#include <functional>

namespace nama::math {

namespace internal {

template <typename Grid>
void check_shape([[maybe_unused]] const Grid& grid0, [[maybe_unused]] const Grid& grid1) {
    if constexpr (Grid::dim == 2) {
        assert(grid0.dx() == grid1.dx());
        assert(grid0.origin() == grid1.origin());
        assert(grid0.ni() == grid1.ni());
        assert(grid0.nj() == grid1.nj());
    } else {
        assert(grid0.dx() == grid1.dx());
        assert(grid0.origin() == grid1.origin());
        assert(grid0.ni() == grid1.ni());
        assert(grid0.nj() == grid1.nj());
        assert(grid0.nk() == grid1.nk());
    }
}

}  // namespace internal

// scg = Constructive Solid Geometry

template <typename Grid>
Grid csg_complement(const Grid& grid) {
    Grid result = grid;
    for (auto& e : result) {
        e = -e;
    }
    return result;
}

template <typename Grid>
Grid csg_union(const Grid& grid0, const Grid& grid1) {
    internal::check_shape(grid0, grid1);
    Grid result = grid0;
    for (size_t i = 0; i < result.size(); ++i) {
        result[i] = std::min(grid0[i], grid1[i]);
    }
    return result;
}

template <typename Grid>
Grid csg_intersection(const Grid& grid0, const Grid& grid1) {
    internal::check_shape(grid0, grid1);
    Grid result = grid0;
    for (size_t i = 0; i < result.size(); ++i) {
        result[i] = std::max(grid0[i], grid1[i]);
    }
    return result;
}

template <typename Grid>
Grid csg_difference(const Grid& grid0, const Grid& grid1) {
    internal::check_shape(grid0, grid1);
    return csg_intersection(grid0, csg_complement(grid1));
}

template <typename T>
inline T fraction_inside(T phi_left, T phi_right) {
    if (phi_left < T{} && phi_right < T{}) {
        return static_cast<T>(1);
    }
    if (phi_left < T{} && phi_right >= T{}) {
        return phi_left / (phi_left - phi_right);
    }
    if (phi_left >= T{} && phi_right < T{}) {
        return phi_right / (phi_right - phi_left);
    }
    return T{};
}

// https://github.com/christopherbatty/Fluid3D
template <typename T>
inline T fraction_inside(T phi_bl, T phi_br, T phi_tl, T phi_tr) {
    auto cycle_array = [](T* arr, int size) {
        T t = arr[0];
        for (int i = 0; i < size - 1; ++i) arr[i] = arr[i + 1];
        arr[size - 1] = t;
    };

    int inside_count = (phi_bl < T{} ? 1 : 0) + (phi_tl < T{} ? 1 : 0) + (phi_br < T{} ? 1 : 0)
                       + (phi_tr < T{} ? 1 : 0);
    T list[] = {phi_bl, phi_br, phi_tr, phi_tl};

    if (inside_count == 4)
        return 1;
    else if (inside_count == 3) {
        // rotate until the positive value is in the first position
        while (list[0] < 0) {
            cycle_array(list, 4);
        }

        // Work out the area of the exterior triangle
        T side0 = 1 - fraction_inside(list[0], list[3]);
        T side1 = 1 - fraction_inside(list[0], list[1]);
        return 1 - 0.5 * side0 * side1;
    } else if (inside_count == 2) {
        // rotate until a negative value is in the first position, and the next negative is in
        // either slot 1 or 2.
        while (list[0] >= 0 || !(list[1] < 0 || list[2] < 0)) {
            cycle_array(list, 4);
        }

        if (list[1] < 0) {  // the matching signs are adjacent
            T side_left  = fraction_inside(list[0], list[3]);
            T side_right = fraction_inside(list[1], list[2]);
            return 0.5 * (side_left + side_right);
        } else {  // matching signs are diagonally opposite
            // determine the centre point's sign to disambiguate this case
            T middle_point = 0.25 * (list[0] + list[1] + list[2] + list[3]);
            if (middle_point < 0) {
                T area = 0;

                // first triangle (top left)
                T side1 = 1 - fraction_inside(list[0], list[3]);
                T side3 = 1 - fraction_inside(list[2], list[3]);

                area += 0.5 * side1 * side3;

                // second triangle (top right)
                T side2 = 1 - fraction_inside(list[2], list[1]);
                T side0 = 1 - fraction_inside(list[0], list[1]);
                area += 0.5 * side0 * side2;

                return 1 - area;
            } else {
                T area = 0;

                // first triangle (bottom left)
                T side0 = fraction_inside(list[0], list[1]);
                T side1 = fraction_inside(list[0], list[3]);
                area += 0.5 * side0 * side1;

                // second triangle (top right)
                T side2 = fraction_inside(list[2], list[1]);
                T side3 = fraction_inside(list[2], list[3]);
                area += 0.5 * side2 * side3;
                return area;
            }
        }
    } else if (inside_count == 1) {
        // rotate until the negative value is in the first position
        while (list[0] >= 0) {
            cycle_array(list, 4);
        }

        // Work out the area of the interior triangle, and subtract from 1.
        T side0 = fraction_inside(list[0], list[3]);
        T side1 = fraction_inside(list[0], list[1]);
        return 0.5 * side0 * side1;
    }
    return 0;
}

template <typename T, typename TV2>
inline T calc_shortest_distance_2d(const TV2& x0, const TV2& x1, const TV2& x2) {
    T a = x2[1] - x1[1];
    T b = -(x2[0] - x1[0]);
    T c = -(x1[0] * x2[1] - x2[0] * x1[1]);
    return std::fabs(a * x0[0] + b * x0[1] + c) / std::sqrt(a * a + b * b);
}

template <typename T, typename TV3>
inline T calc_shortest_distance_3d(const TV3& x0, const TV3& x1, const TV3& x2) {
    T area = (x0 - x1).cross(x2 - x1).norm();
    T l    = (x2 - x1).norm();
    return area / l;
}

template <typename T, typename TV3>
inline T calc_shortest_distance_3d(const TV3& x0, const TV3& x1, const TV3& x2, const TV3& x3) {
    TV3 u = x2 - x1;
    TV3 v = x3 - x1;
    TV3 n = u.cross(v);
    T a   = n[0];
    T b   = n[1];
    T c   = n[2];
    T d   = -(n[0] * x1[0] + n[1] * x1[1] + n[2] * x1[2]);
    return std::fabs(a * x0[0] + b * x0[1] + c * x0[2] + d) / std::sqrt(a * a + b * b + c * c);
}

template <typename T, typename TV>
inline T calc_distance_from_zero(const TV& x0, const TV& x1, const TV& x2, T phi0, T phi1, T phi2) {
    if (phi0 == T{}) {
        return T{};
    }
    std::vector<TV> x_intersection;
    if (phi0 * phi1 <= T{}) {
        x_intersection.push_back(vertex_interp(x0, x1, phi0, phi1, T{}));
    }
    if (phi0 * phi2 <= T{}) {
        x_intersection.push_back(vertex_interp(x0, x2, phi0, phi2, T{}));
    }
    if (x_intersection.size() == 1) {
        return (x_intersection[0] - x0).norm();
    } else if (x_intersection.size() == 2) {
        return calc_shortest_distance_2d<T>(x0, x_intersection[0], x_intersection[1]);
    }
    return std::numeric_limits<T>::max();
}

template <typename T, typename TV>
inline T calc_distance_from_zero(const TV& x0, const TV& x1, const TV& x2, const TV& x3, T phi0,
                                 T phi1, T phi2, T phi3) {
    if (phi0 == T{}) {
        return T{};
    }
    std::vector<TV> x_intersection;
    if (phi0 * phi1 <= T{}) {
        x_intersection.push_back(vertex_interp(x0, x1, phi0, phi1, T{}));
    }
    if (phi0 * phi2 <= T{}) {
        x_intersection.push_back(vertex_interp(x0, x2, phi0, phi2, T{}));
    }
    if (phi0 * phi3 <= T{}) {
        x_intersection.push_back(vertex_interp(x0, x3, phi0, phi3, T{}));
    }
    if (x_intersection.size() == 1) {
        return (x_intersection[0] - x0).norm();
    } else if (x_intersection.size() == 2) {
        return calc_shortest_distance_3d<T>(x0, x_intersection[0], x_intersection[1]);
    } else if (x_intersection.size() == 3) {
        return calc_shortest_distance_3d<T>(x0, x_intersection[0], x_intersection[1],
                                            x_intersection[2]);
    }
    return std::numeric_limits<T>::max();
}

template <typename T, typename TV>
inline T calc_sdf(const std::array<TV, 5>& x_arr, const std::array<T, 5>& phi_arr) {
    T l                      = std::numeric_limits<T>::max();
    std::array<int, 5> c_arr = {1, 2, 3, 4, 1};
    for (int i = 0; i < 4; ++i) {
        l = std::min(l, calc_distance_from_zero(
                            x_arr[0], x_arr.at(c_arr.at(i)), x_arr.at(c_arr.at(i + 1)), phi_arr[0],
                            phi_arr.at(c_arr.at(i)), phi_arr.at(c_arr.at(i + 1))));
    }
    return phi_arr[0] < 0 ? -l : l;
}

template <typename T, typename TV>
inline T calc_sdf(const std::array<TV, 7>& x_arr, const std::array<T, 7>& phi_arr) {
    T l                      = std::numeric_limits<T>::max();
    std::array<int, 5> c_arr = {1, 2, 3, 4, 1};
    for (int i = 0; i < 4; ++i) {
        for (int j : {5, 6}) {
            l = std::min(l, calc_distance_from_zero(x_arr[0], x_arr.at(c_arr.at(i)),
                                                    x_arr.at(c_arr.at(i + 1)), x_arr.at(j),
                                                    phi_arr[0], phi_arr.at(c_arr.at(i)),
                                                    phi_arr.at(c_arr.at(i + 1)), phi_arr.at(j)));
        }
    }
    return phi_arr[0] < 0 ? -l : l;
}

}  // namespace nama::math
