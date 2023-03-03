#pragma once

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <filesystem>

namespace nama::graphics {

class Window {
   public:
    Window() = default;
    explicit Window(const std::string& window_name);
    Window(const std::string& window_name, int width, int height);
    ~Window();

    void set_name(const std::string& window_name);
    std::string get_name() const;

    void set_width(int width);
    int get_width() const;
    void set_height(int height);
    int get_height() const;

    void create(int pos_x, int pos_y);
    void make_context_current() const;
    void save_image(const std::filesystem::path& output) const;
    void swap_buffer() const;
    bool should_close() const;

    void set_color(float r, float g, float b);

   private:
    std::string m_window_name;
    GLFWwindow* m_window_ptr = nullptr;
    int m_width;
    int m_height;

    float m_r = 0.0, m_g = 0.0, m_b = 0.0;
};

}  // namespace nama::graphics
