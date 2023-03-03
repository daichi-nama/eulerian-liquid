#pragma once

#include <nama/common/grid.h>
#include <nama/common/staggered_grid.h>

namespace nama::_3d::fluid {

void compute_staggered_grid_weight(StaggeredGrid3f& weight, const Grid3f& nodal_solid_level_set);

}  // namespace nama::_3d::fluid
