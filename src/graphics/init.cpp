#include <nama/graphics/init.h>
#include <stb_image.h>

#include <cassert>
#include <iostream>

namespace nama::graphics {

void init_glfw() {
    if (glfwInit() == GL_FALSE) {
        throw std::runtime_error("glfwInit() failed!");
    }
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    if (std::atexit(glfwTerminate) != 0) {
        throw std::runtime_error("std::atexit(glfwTerminate) failed!");
    }
}

void set_texture(const std::filesystem::path& texture_path, GLuint& texture_id) {
    int tex_width, tex_height, channels;
    unsigned char* bytes
        = stbi_load(texture_path.c_str(), &tex_width, &tex_height, &channels, STBI_rgb_alpha);
    if (!bytes) {
        throw std::runtime_error("Failed to load image file: " + texture_path.string());
    }
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 bytes);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(bytes);
}

void enable_capabilities_2d() {
    glEnable(GL_TEXTURE_2D);
}

void enable_capabilities_2d_opacity() {
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void enable_capabilities_3d() {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
}

void enable_capabilities_3d_lighting() {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
    // glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    // glDisable(GL_CULL_FACE);
}

}  // namespace nama::graphics
