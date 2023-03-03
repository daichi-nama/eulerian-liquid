#pragma once

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <filesystem>

namespace nama::graphics {

void init_glfw();
void set_texture(const std::filesystem::path& texture_path, GLuint& texture_id);

void enable_capabilities_2d();
void enable_capabilities_2d_opacity();

void enable_capabilities_3d();
void enable_capabilities_3d_lighting();

}  // namespace nama::graphics
