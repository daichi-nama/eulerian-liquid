#include <nama/math/interpolation.h>
#include <nama/math/ode.h>
#include <nama/project/3d/fluid/advection.h>

namespace nama::_3d::fluid {

using Eigen::Vector3f;

void advect_collocated_grid(Grid3f& grid, const Grid3f& grid_temp, const StaggeredGrid3f& velocity,
                            float dt) {
    StaggeredGridField<StaggeredGrid3f, Vector3f> field(velocity);
    parallel_loop(grid, [&](int i, int j, int k) {
        Vector3f pos  = math::trace_rk2(grid.pos(i, j, k), -dt, field);
        grid(i, j, k) = math::lerp(pos, grid_temp);
    });
}

void advect_staggered_grid(StaggeredGrid3f& grid, const StaggeredGrid3f& grid_temp,
                           const StaggeredGrid3f& velocity, float dt) {
    StaggeredGridField<StaggeredGrid3f, Vector3f> field(velocity);
    for (int d = 0; d < grid.dim; ++d) {
        parallel_loop(grid[d], [&](int i, int j, int k) {
            Vector3f pos     = math::trace_rk2(grid[d].pos(i, j, k), -dt, field);
            grid[d](i, j, k) = math::lerp(pos, grid_temp[d]);
        });
    }
}

}  // namespace nama::_3d::fluid
