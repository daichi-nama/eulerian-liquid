#include <nama/graphics/draw/grid.h>
#include <nama/math/geometry.h>
#include <nama/math/utils.h>
#include <nama/project/3d/eulerian_liquid/visualizer/02_contour_2d.h>

namespace nama::_3d::eulerian_liquid {

using Eigen::Vector3f;

static graphics::CameraOrtho create_camera(const Grid3f& level_set, float margin) {
    Eigen::Vector3f origin = level_set.origin();
    float li               = level_set.li();
    float lj               = level_set.lj();
    float lk               = level_set.lk();
    glm::vec3 min_bound(origin(0), origin(1), origin(2));
    glm::vec3 max_bound(li + origin(0), lj + origin(1), lk + origin(2));
    return graphics::create_camera_for_2d_visualization(min_bound, max_bound, "xy", margin);
}

ContourVisualizer2d::ContourVisualizer2d(Simulator* sim,
                                         const std::vector<std::string>& image_names,
                                         const std::vector<std::string>& file_names,
                                         const std::string& window_name, int window_height,
                                         int watching_index)
    : VisualizerBase(sim)
    , OpenglVisualizer(image_names, file_names, window_name, window_height,
                       std::make_unique<graphics::CameraOrtho>(create_camera(sim->m_level_set, 0)))
    , m_watching_index(watching_index) {}

void ContourVisualizer2d::enable_capabilities() const {
    graphics::enable_capabilities_3d();
}

void ContourVisualizer2d::init_texture() {
    graphics::set_texture(ASSET_DIR / "heat.png", m_texture_id);
}

void ContourVisualizer2d::visualize() {
    m_window.make_context_current();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    graphics::draw_axis_3d();

    auto calc_tex_coord = [=](float phi) { return math::min_max_normalization(phi, -0.2f, 0.2f); };
    glColor3f(1, 1, 1);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    for (float isolevel = -1.0; isolevel <= 1.0; isolevel += 0.1) {
        glTexCoord2f(calc_tex_coord(isolevel), 0.5);
        graphics::draw_mesh(m_sim->m_level_set.slice_k(m_watching_index), isolevel, true);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    m_window.save_image(get_image_path(0, "png"));
    m_window.swap_buffer();
}

}  // namespace nama::_3d::eulerian_liquid
