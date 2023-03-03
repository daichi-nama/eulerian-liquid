#pragma once

#include <nama/common/grid.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

namespace nama::graphics {

void draw_mesh(const Grid2f& grid, float isolevel, bool only_contour);

}  // namespace nama::graphics
