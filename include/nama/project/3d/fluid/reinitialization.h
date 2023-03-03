#pragma once

#include <nama/common/grid.h>

namespace nama::_3d::fluid {

void reinitialize_fast_marching(Grid3f& grid, Grid3f& grid_temp, const Grid3c& valid);

}  // namespace nama::_3d::fluid
