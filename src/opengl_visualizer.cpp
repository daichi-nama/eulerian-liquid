#include <nama/opengl_visualizer.h>

#include <algorithm>
#include <fstream>
#include <iostream>

namespace nama {

OpenglVisualizer::OpenglVisualizer(const std::vector<std::string>& image_names,
                                   const std::vector<std::string>& file_names)
    : AbstractVisualizer(image_names, file_names) {}

OpenglVisualizer::OpenglVisualizer(const std::vector<std::string>& image_names,
                                   const std::vector<std::string>& file_names,
                                   const std::string& window_name, int window_height,
                                   std::unique_ptr<graphics::CameraBase>&& camera_ptr)
    : AbstractVisualizer(image_names, file_names)
    , m_camera_ptr(std::move(camera_ptr))
    , m_window(window_name, window_height * m_camera_ptr->get_aspect(), window_height) {}

void OpenglVisualizer::init() {
    create_window();
    m_window.make_context_current();
    init_texture();
    set_camera();
    enable_capabilities();
    init_lights();

    create_window_name_text(0);
}

void OpenglVisualizer::create_window() {
    static int pos_x           = 100;
    static int pos_y           = 100;
    static constexpr int space = 10;
    static std::vector<int> heights;
    if (pos_x + m_window.get_width() >= 1920) {
        auto max_height = *std::max_element(heights.begin(), heights.end());
        pos_y += max_height + space * 5;
        pos_x = 100;
        heights.clear();
    }
    heights.push_back(m_window.get_height());
    m_window.create(pos_x, pos_y);
    pos_x += m_window.get_width() + space;
}

void OpenglVisualizer::init_texture() {}

void OpenglVisualizer::set_camera() const {
    if (m_camera_ptr) {
        m_camera_ptr->set();
    }
}

void OpenglVisualizer::init_lights() const {
    m_material_set.enable_lights();
}

bool OpenglVisualizer::should_close() const {
    return m_window.should_close();
}

std::string OpenglVisualizer::get_window_name() const {
    return m_window.get_name();
}

void OpenglVisualizer::set_window_name(const std::string& name) {
    m_window.set_name(name);
}

void OpenglVisualizer::create_window_name_text(int i) const {
    std::ofstream file;
    auto path = s_output / get_image_directory(i) / "01_window_name.txt";
    file.open(path);
    if (file.is_open()) {
        file << m_window.get_name();
        file.close();
    } else {
        throw std::runtime_error("File '" + path.string() + "' not open.");
    }
}

}  // namespace nama
