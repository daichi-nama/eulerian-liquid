#include <nama/common/meshing.h>
#include <nama/graphics/draw/mesh.h>
#include <nama/math/geometry.h>
#include <nama/math/level_set.h>
#include <nama/project/3d/eulerian_liquid/visualizer/01_marching_cubes.h>

namespace nama::_3d::eulerian_liquid {

MarchingCubesVisualizer::MarchingCubesVisualizer(Simulator* sim,
                                                 const std::vector<std::string>& image_names,
                                                 const std::vector<std::string>& file_names,
                                                 const std::string& window_name, int window_height,
                                                 const graphics::MaterialSet& material_set,
                                                 std::unique_ptr<graphics::CameraBase>&& camera_ptr)
    : VisualizerBase(sim)
    , OpenglVisualizer(image_names, file_names, window_name, window_height, std::move(camera_ptr)) {
    m_material_set = material_set;
}

void MarchingCubesVisualizer::enable_capabilities() const {
    graphics::enable_capabilities_3d_lighting();

    // glDisable(GL_CULL_FACE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}

void MarchingCubesVisualizer::visualize() {
    using Eigen::Vector3f;

    m_window.make_context_current();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    graphics::draw_axis_3d();

    glEnable(GL_LIGHTING);
    m_material_set.set_material(0);
    Mesh3f mesh
        = get_mesh(math::csg_difference(m_sim->m_level_set, m_sim->m_cell_centered_solid_level_set),
                   0.0f, MeshingMethod::MARCHING_CUBES);
    graphics::draw_mesh(mesh);
    glDisable(GL_LIGHTING);

    m_window.save_image(get_image_path(0, "png"));
    m_window.swap_buffer();

    mesh.save(get_file_path(0, "obj"));
}

}  // namespace nama::_3d::eulerian_liquid
