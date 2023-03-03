#pragma once

#include <nama/common/grid.h>
#include <nama/common/staggered_grid.h>

#include <Eigen/SparseCore>

namespace nama::_3d::fluid {

int assign_indices(Grid3i& indices, const Grid3f& level_set,
                   const StaggeredGrid3f& velocity_weight);

void build_matrix(Eigen::SparseMatrix<double, Eigen::RowMajor>& A, Eigen::VectorXd& b,
                  const int mat_size, const Grid3i& indices, const Grid3f& level_set,
                  const StaggeredGrid3f& velocity, const StaggeredGrid3f& velocity_weight,
                  const float dt);

void build_grid_from_vector(Grid3f& grid, const Eigen::VectorXd& vector, const Grid3i& indices,
                            float background_value);

void update_velocity_using_pressure(StaggeredGrid3f& velocity, const Grid3f& pressure,
                                    const Grid3f& level_set, float dt);

}  // namespace nama::_3d::fluid
