#include <nama/graphics/init.h>
#include <nama/simulator_base.h>

#include <algorithm>

namespace nama {

void SimulatorBase::create_result_directories() const {
    for (const auto& vis : m_vis_arr) {
        vis->create_result_directories();
    }
}

void SimulatorBase::init_visualizers() const {
    graphics::init_glfw();
    for (const auto& vis : m_vis_arr) {
        vis->init();
    }
}

void SimulatorBase::visualize() {
    for (const auto& vis : m_vis_arr) {
        vis->visualize();
        vis->increment();
    }
    ++m_frame_count;
    glfwPollEvents();
}

void SimulatorBase::deinit_visualizers() const {
    for (const auto& vis : m_vis_arr) {
        vis->generate_movie();
    }
}

bool SimulatorBase::should_break() const {
    return std::any_of(m_vis_arr.begin(), m_vis_arr.end(),
                       [](const auto& vis) { return vis->should_close(); });
}

void SimulatorBase::add_visualizer(std::unique_ptr<AbstractVisualizer>&& vis) {
    m_vis_arr.push_back(std::move(vis));
}

}  // namespace nama
