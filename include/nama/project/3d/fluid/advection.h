#pragma once

#include <nama/common/grid.h>
#include <nama/common/staggered_grid.h>

namespace nama::_3d::fluid {

void advect_collocated_grid(Grid3f& grid, const Grid3f& grid_temp, const StaggeredGrid3f& velocity,
                            float dt);
void advect_staggered_grid(StaggeredGrid3f& grid, const StaggeredGrid3f& grid_temp,
                           const StaggeredGrid3f& velocity, float dt);

}  // namespace nama::_3d::fluid
