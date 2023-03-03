#include <nama/graphics/window.h>
#include <stb_image_write.h>

#include <cassert>
#include <iostream>
#include <vector>

namespace nama::graphics {

namespace fs = std::filesystem;

Window::Window(const std::string& window_name) : m_window_name(window_name) {}

Window::Window(const std::string& window_name, int width, int height)
    : m_window_name(window_name), m_width(width), m_height(height) {}

Window::~Window() {
    if (m_window_ptr != nullptr) {
        glfwDestroyWindow(m_window_ptr);
    }
}

void Window::set_name(const std::string& window_name) {
    m_window_name = window_name;
}

std::string Window::get_name() const {
    return m_window_name;
}

void Window::set_width(int width) {
    m_width = width;
}

int Window::get_width() const {
    return m_width;
}

void Window::set_height(int height) {
    m_height = height;
}

int Window::get_height() const {
    return m_height;
}

void Window::create(int pos_x, int pos_y) {
    m_window_ptr = glfwCreateWindow(m_width, m_height, m_window_name.c_str(), nullptr, nullptr);
    if (m_window_ptr == nullptr) {
        throw std::runtime_error("Window creation failed!");
    }
    glfwSetWindowPos(m_window_ptr, pos_x, pos_y);
    glfwMakeContextCurrent(m_window_ptr);
    glClearColor(m_r, m_g, m_b, 1.0);
}

void Window::make_context_current() const {
    glfwMakeContextCurrent(m_window_ptr);
}

void Window::save_image(const fs::path& output) const {
    int width, height;
    glfwGetFramebufferSize(m_window_ptr, &width, &height);
    std::vector<unsigned char> buffer(4 * width * height);

    glFlush();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());

    for (int j = 0; j != height / 2; ++j) {
        std::swap_ranges(buffer.begin() + 4 * width * j, buffer.begin() + 4 * width * (j + 1),
                         buffer.begin() + 4 * width * (height - j - 1));
    }

    stbi_write_png(output.c_str(), width, height, 4, buffer.data(), 0);
}

void Window::swap_buffer() const {
    glfwSwapBuffers(m_window_ptr);
}

bool Window::should_close() const {
    return glfwWindowShouldClose(m_window_ptr);
}

void Window::set_color(float r, float g, float b) {
    m_r = r;
    m_g = g;
    m_b = b;
}

}  // namespace nama::graphics
