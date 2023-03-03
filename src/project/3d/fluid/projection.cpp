#include <nama/math/level_set.h>
#include <nama/project/3d/fluid/projection.h>

namespace nama::_3d::fluid {

static constexpr int UNKNOWN_INDEX = std::numeric_limits<int>::max();

static bool is_pressure_valid(const StaggeredGrid3f& velocity_weight, int i, int j, int k) {
    return velocity_weight[0](i, j, k) > 0 || velocity_weight[0](i + 1, j, k) > 0     //
           || velocity_weight[1](i, j, k) > 0 || velocity_weight[1](i, j + 1, k) > 0  //
           || velocity_weight[2](i, j, k) > 0 || velocity_weight[2](i, j, k + 1) > 0;
}

int assign_indices(Grid3i& indices, const Grid3f& level_set,
                   const StaggeredGrid3f& velocity_weight) {
    int mat_size = 0;
    serial_loop(level_set, [&](int i, int j, int k) {
        if (is_pressure_valid(velocity_weight, i, j, k) && level_set(i, j, k) < 0) {
            indices(i, j, k) = mat_size++;
        } else {
            indices(i, j, k) = UNKNOWN_INDEX;
        }
    });
    if (mat_size == 0) {
        throw std::runtime_error("No fluid");
    }
    return mat_size;
}

void build_matrix(Eigen::SparseMatrix<double, Eigen::RowMajor>& A, Eigen::VectorXd& b,
                  const int mat_size, const Grid3i& indices, const Grid3f& level_set,
                  const StaggeredGrid3f& velocity, const StaggeredGrid3f& velocity_weight,
                  const float dt) {
    auto& u        = velocity.u();
    auto& v        = velocity.v();
    auto& w        = velocity.w();
    auto& u_weight = velocity_weight.u();
    auto& v_weight = velocity_weight.v();
    auto& w_weight = velocity_weight.w();

    A.resize(mat_size, mat_size);
    b.resize(mat_size);

    std::vector<Eigen::Triplet<double>> triplet_vec;
    serial_loop(level_set, [&](int i, int j, int k) {
        const int index = indices(i, j, k);
        if (index == UNKNOWN_INDEX) {
            return;
        }
        b(index)            = 0.0;
        double diag_element = 0.0;
        auto add_matrix_element
            = [&](int x, int y, int z, float vel, float weight, bool is_forward) -> void {
            if (is_pressure_valid(velocity_weight, x, y, z)) {
                if (level_set(x, y, z) < 0) {
                    diag_element += weight;
                    assert(indices(x, y, z) != UNKNOWN_INDEX);
                    triplet_vec.push_back(Eigen::Triplet<double>(index, indices(x, y, z), -weight));
                } else {
                    float theta = math::fraction_inside(level_set(i, j, k), level_set(x, y, z));
                    theta       = std::max(theta, 0.01f);
                    diag_element += weight / theta;
                }
            }
            if (is_forward) {
                b(index) -= weight * vel;
            } else {
                b(index) += weight * vel;
            }
        };
        add_matrix_element(i - 1, j, k, u(i + 0, j, k), u_weight(i + 0, j, k), false);
        add_matrix_element(i + 1, j, k, u(i + 1, j, k), u_weight(i + 1, j, k), true);
        add_matrix_element(i, j - 1, k, v(i, j + 0, k), v_weight(i, j + 0, k), false);
        add_matrix_element(i, j + 1, k, v(i, j + 1, k), v_weight(i, j + 1, k), true);
        add_matrix_element(i, j, k - 1, w(i, j, k + 0), w_weight(i, j, k + 0), false);
        add_matrix_element(i, j, k + 1, w(i, j, k + 1), w_weight(i, j, k + 1), true);
        triplet_vec.push_back(
            Eigen::Triplet<double>(indices(i, j, k), indices(i, j, k), diag_element));
    });

    A.setFromTriplets(triplet_vec.begin(), triplet_vec.end());
    b *= level_set.dx() / dt;
}

void build_grid_from_vector(Grid3f& grid, const Eigen::VectorXd& vector, const Grid3i& indices,
                            float background_value) {
    parallel_loop(grid, [&](int i, int j, int k) {
        grid(i, j, k)
            = indices(i, j, k) == UNKNOWN_INDEX ? background_value : vector(indices(i, j, k));
    });
}

void update_velocity_using_pressure(StaggeredGrid3f& velocity, const Grid3f& pressure,
                                    const Grid3f& level_set, float dt) {
    auto& u = velocity.u();
    auto& v = velocity.v();
    auto& w = velocity.w();
    parallel_loop<int>  //
        ({1, u.ni() - 1}, {1, u.nj() - 1}, {1, u.nk() - 1}, [&](int i, int j, int k) {
            if (level_set(i, j, k) < 0 || level_set(i - 1, j, k) < 0) {
                float theta = 1;
                if (level_set(i, j, k) >= 0 || level_set(i - 1, j, k) >= 0) {
                    theta = math::fraction_inside(level_set(i - 1, j, k), level_set(i, j, k));
                }
                theta = std::max(theta, 0.01f);
                u(i, j, k)
                    -= dt * (pressure(i, j, k) - pressure(i - 1, j, k)) / (pressure.dx() * theta);
            }
        });
    parallel_loop<int>  //
        ({1, v.ni() - 1}, {1, v.nj() - 1}, {1, v.nk() - 1}, [&](int i, int j, int k) {
            if (level_set(i, j, k) < 0 || level_set(i, j - 1, k) < 0) {
                float theta = 1;
                if (level_set(i, j, k) >= 0 || level_set(i, j - 1, k) >= 0) {
                    theta = math::fraction_inside(level_set(i, j - 1, k), level_set(i, j, k));
                }
                theta = std::max(theta, 0.01f);
                v(i, j, k)
                    -= dt * (pressure(i, j, k) - pressure(i, j - 1, k)) / (pressure.dx() * theta);
            }
        });
    parallel_loop<int>  //
        ({1, w.ni() - 1}, {1, w.nj() - 1}, {1, w.nk() - 1}, [&](int i, int j, int k) {
            if (level_set(i, j, k) < 0 || level_set(i, j, k - 1) < 0) {
                float theta = 1;
                if (level_set(i, j, k) >= 0 || level_set(i, j, k - 1) >= 0) {
                    theta = math::fraction_inside(level_set(i, j, k - 1), level_set(i, j, k));
                }
                theta = std::max(theta, 0.01f);
                w(i, j, k)
                    -= dt * (pressure(i, j, k) - pressure(i, j, k - 1)) / (pressure.dx() * theta);
            }
        });
}

}  // namespace nama::_3d::fluid
