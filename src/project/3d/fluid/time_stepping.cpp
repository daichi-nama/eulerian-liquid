#include <nama/project/3d/fluid/time_stepping.h>

namespace nama::_3d::fluid {

float calc_time_step(const float max_cfl, const StaggeredGrid3f& grid) {
    return max_cfl * grid.dx() / std::max(grid.max_absolute_value(), 1e-4f);
}

}  // namespace nama::_3d::fluid
