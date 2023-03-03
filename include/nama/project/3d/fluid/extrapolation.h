#pragma once

#include <nama/common/grid.h>

namespace nama::_3d::fluid {

void extrapolate(Array3f& grid, Array3c& valid, int layers);
void extrapolate_const(Array3f& grid, const Array3c& valid, int layers);

}  // namespace nama::_3d::fluid
