#include <nama/math/level_set.h>
#include <nama/project/3d/fluid/reinitialization.h>

namespace nama::_3d::fluid {

namespace internal {

enum class Label {
    FIX,
    NEAR,
    FAR,

    UNKNOWN,
};

}  // namespace internal

void reinitialize_fast_marching(Grid3f& grid, Grid3f& grid_temp, const Grid3c& valid) {
    using namespace internal;
    using namespace Eigen;

    float dx = grid.dx();
    Grid3<Label> labels;
    labels.reshape_like(grid);
    grid_temp = grid;
    serial_loop(grid, [&](int i, int j, int k) {
        if (valid(i, j, k) == 0) {
            labels(i, j, k) = Label::UNKNOWN;
            return;
        }
        bool flag = grid_temp(i, j, k) * grid_temp.extrapolate(i - 1, j, k) <= 0     //
                    || grid_temp(i, j, k) * grid_temp.extrapolate(i + 1, j, k) <= 0  //
                    || grid_temp(i, j, k) * grid_temp.extrapolate(i, j - 1, k) <= 0  //
                    || grid_temp(i, j, k) * grid_temp.extrapolate(i, j + 1, k) <= 0  //
                    || grid_temp(i, j, k) * grid_temp.extrapolate(i, j, k - 1) <= 0  //
                    || grid_temp(i, j, k) * grid_temp.extrapolate(i, j, k + 1) <= 0;
        if (flag) {
            if (grid(i, j, k) != 0) {
                std::array<Vector3f, 7> x_arr
                    = {Vector3f((i + 0.5) * dx, (j + 0.5) * dx, (k + 0.5) * dx),
                       Vector3f(((i + 1) + 0.5) * dx, (j + 0.5) * dx, (k + 0.5) * dx),
                       Vector3f((i + 0.5) * dx, ((j + 1) + 0.5) * dx, (k + 0.5) * dx),
                       Vector3f(((i - 1) + 0.5) * dx, (j + 0.5) * dx, (k + 0.5) * dx),
                       Vector3f((i + 0.5) * dx, ((j - 1) + 0.5) * dx, (k + 0.5) * dx),
                       Vector3f((i + 0.5) * dx, (j + 0.5) * dx, ((k + 1) + 0.5) * dx),
                       Vector3f((i + 0.5) * dx, (j + 0.5) * dx, ((k - 1) + 0.5) * dx)};
                std::array<float, 7> phi_arr = {
                    grid_temp.extrapolate(i, j, k),     grid_temp.extrapolate(i + 1, j, k),
                    grid_temp.extrapolate(i, j + 1, k), grid_temp.extrapolate(i - 1, j, k),
                    grid_temp.extrapolate(i, j - 1, k), grid_temp.extrapolate(i, j, k + 1),
                    grid_temp.extrapolate(i, j, k - 1),
                };
                grid(i, j, k) = math::calc_sdf(x_arr, phi_arr);
            }
            labels(i, j, k) = Label::FIX;
        } else {
            labels(i, j, k) = Label::FAR;
        }
    });

    while (true) {
        bool should_return = true;
        grid_temp          = grid;
        serial_loop(grid, [&](int i, int j, int k) {
            if (labels(i, j, k) == Label::FAR) {
                should_return = false;
                if (labels.extrapolate(i - 1, j, k) == Label::FIX
                    || labels.extrapolate(i + 1, j, k) == Label::FIX
                    || labels.extrapolate(i, j - 1, k) == Label::FIX
                    || labels.extrapolate(i, j + 1, k) == Label::FIX
                    || labels.extrapolate(i, j, k - 1) == Label::FIX
                    || labels.extrapolate(i, j, k + 1) == Label::FIX) {
                    labels(i, j, k) = Label::NEAR;
                }
            }
        });
        if (should_return) {
            return;
        }
        serial_loop(grid, [&](int i, int j, int k) {
            if (labels(i, j, k) == Label::NEAR) {
                std::vector<float> phi_arr;
                if (labels.extrapolate(i - 1, j, k) == Label::FIX) {
                    phi_arr.push_back(grid_temp.extrapolate(i - 1, j, k));
                } else if (labels.extrapolate(i + 1, j, k) == Label::FIX) {
                    phi_arr.push_back(grid_temp.extrapolate(i + 1, j, k));
                }
                if (labels.extrapolate(i, j - 1, k) == Label::FIX) {
                    phi_arr.push_back(grid_temp.extrapolate(i, j - 1, k));
                } else if (labels.extrapolate(i, j + 1, k) == Label::FIX) {
                    phi_arr.push_back(grid_temp.extrapolate(i, j + 1, k));
                }
                if (labels.extrapolate(i, j, k - 1) == Label::FIX) {
                    phi_arr.push_back(grid_temp.extrapolate(i, j, k - 1));
                } else if (labels.extrapolate(i, j, k + 1) == Label::FIX) {
                    phi_arr.push_back(grid_temp.extrapolate(i, j, k + 1));
                }
                assert(phi_arr.size() != 0);
                float d  = 0;
                int mode = -1;
                if (std::all_of(phi_arr.begin(), phi_arr.end(),
                                [](float phi) { return phi >= 0; })) {
                    mode = 0;
                } else if (std::all_of(phi_arr.begin(), phi_arr.end(),
                                       [](float phi) { return phi <= 0; })) {
                    mode = 1;
                } else {
                    assert(false);
                }
                if (mode == 1) {
                    for (auto& phi : phi_arr) {
                        phi = -phi;
                    }
                }
                std::sort(phi_arr.begin(), phi_arr.end());
                assert(phi_arr.size() == 1 || phi_arr.size() == 2 || phi_arr.size() == 3);
                d = phi_arr[0] + dx;
                if (phi_arr.size() >= 2) {
                    if (d > phi_arr[1]) {
                        d = 0.5
                            * (phi_arr[0] + phi_arr[1]
                               + std::sqrt(2 * dx * dx
                                           - (phi_arr[1] - phi_arr[0])
                                                 * (phi_arr[1] - phi_arr[0])));
                    }
                }
                if (phi_arr.size() == 3) {
                    if (d > phi_arr[2]) {
                        d = 0.333333
                            * (phi_arr[0] + phi_arr[1] + phi_arr[2]
                               + std::sqrt(std::max(
                                   0.0f,
                                   (phi_arr[0] + phi_arr[1] + phi_arr[2])
                                           * (phi_arr[0] + phi_arr[1] + phi_arr[2])
                                       - 3.0f
                                             * (phi_arr[0] * phi_arr[0] + phi_arr[1] * phi_arr[1]
                                                + phi_arr[2] * phi_arr[2] - dx * dx))));
                    }
                }
                assert(d >= 0);
                if (mode == 1) {
                    d = -d;
                }
                // if (std::fabs(d) < std::fabs(grid_temp(i, j, k))) {
                grid(i, j, k) = d;
                // }
            }
        });

        serial_loop(grid, [&](int i, int j, int k) {
            if (labels(i, j, k) == Label::NEAR) {
                labels(i, j, k) = Label::FIX;
            }
        });
    }
}

}  // namespace nama::_3d::fluid
