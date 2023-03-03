#pragma once

#include <nama/common/grid.h>
#include <nama/common/staggered_grid.h>

#include <algorithm>

namespace nama::_3d::fluid {

float calc_time_step(const float max_cfl, const StaggeredGrid3f& grid);

}  // namespace nama::_3d::fluid
