#include <nama/output_config.h>
#include <nama/path.h>
#include <nama/project/3d/eulerian_liquid/simulator.h>
#include <nama/project/3d/eulerian_liquid/visualizer/01_marching_cubes.h>
#include <nama/project/3d/eulerian_liquid/visualizer/02_contour_2d.h>
#include <nama/utils/logger.h>
#include <nama/utils/timer.h>
#include <nama/utils/writer.h>

#include <iostream>

#include "cxxopts.hpp"
#include "settings.h"

using namespace nama;
using namespace nama::_3d;
using namespace nama::_3d::eulerian_liquid;
using namespace nama::_3d::fluid;
using namespace nama::graphics;
using namespace nama::utils;

using Eigen::Vector3f;

void set_sim_params(Simulator& sim, const cxxopts::ParseResult& params) {
    SimulationParams p;
    p.cfl = params["cfl"].as<float>();
    sim.set_params(p);
}

// scenes & visualizer

MaterialSet create_material() {
    MaterialSet result;

    Light light0;
    light0.set_position(0.5, 0.5, 1.5, 0.0);
    result.add_light(light0);

    Material material0;
    material0.set_ambient(0.24725, 0.1995, 0.0745, 1.0);
    material0.set_diffuse(0.0, 0.4, 0.8, 1.0);
    material0.set_specular(0.628281, 0.555802, 0.366065, 1.0);
    material0.set_shininess(200);
    result.add_material(material0);

    return result;
}

void scene_water_drop(Simulator& sim, int quality) {
    sim.create_grid(1, quality, quality, quality);
    Vector3f center0(0.25, 0.6, 0.25);
    Vector3f center1(0.5, 0.5, 0.5);
    Vector3f center2(0.75, 0.7, 0.75);
    float radius      = 0.16;
    float pool_height = 0.25;
    Scene scene;
    scene.add_liquid_emitter(
        std::make_shared<LevelSetEmitterFunction>(function::sphere(center0, radius)));
    scene.add_liquid_emitter(
        std::make_shared<LevelSetEmitterFunction>(function::sphere(center1, radius)));
    scene.add_liquid_emitter(
        std::make_shared<LevelSetEmitterFunction>(function::sphere(center2, radius)));
    scene.add_liquid_emitter(
        std::make_shared<LevelSetEmitterFunction>(function::pool(pool_height)));
    scene.add_solid_emitter(std::make_shared<LevelSetEmitterGrid>(function::room(
        sim.m_nodal_solid_phi.ni(), sim.m_nodal_solid_phi.nj(), sim.m_nodal_solid_phi.nk())));
    sim.set_scene(scene);
}

void create_scene_and_camera(Simulator& sim, const cxxopts::ParseResult& params) {
    std::string scene_name               = params["scene"].as<std::string>();
    int quality                          = params["quality"].as<int>();
    float aspect                         = params["aspect"].as<float>();
    std::vector<std::string> image_names = {"marching_cubes_gl"};
    std::vector<std::string> file_names  = {"marching_cubes"};

    CameraPerspective camera(aspect);
    if (scene_name == "water_drop") {
        scene_water_drop(sim, quality);
        camera.set_eye(0.5, 0.6, 3);
        camera.set_center(0.5, 0.5, 0.5);
        camera.set_up(0, 1, 0);
    } else {
        std::cerr << "invalid scene: " << scene_name << std::endl;
        std::exit(1);
    }

    sim.add_visualizer(std::make_unique<MarchingCubesVisualizer>(
        &sim, image_names, file_names, "mesh view", params["window_height"].as<int>(),
        create_material(), std::make_unique<CameraPerspective>(camera)));
}

void set_visualizers(Simulator& sim, [[maybe_unused]] const cxxopts::ParseResult& params) {
    {
        std::vector<std::string> image_names = {"contour_2d_gl"};
        std::vector<std::string> file_names  = {};
        sim.add_visualizer(std::make_unique<ContourVisualizer2d>(
            &sim, image_names, file_names, "contour view 2d", params["window_height"].as<int>(),
            sim.m_level_set.nk() / 2));
    }
    sim.create_result_directories();
}

// update function

void update_second_order(Simulator& sim, float dt) {
    sim.extrapolate_velocity();
    sim.extrapolate_level_set();

    sim.advect_level_set(dt);
    sim.advect_velocity(dt);

    sim.reinitialize();

    sim.add_gravity(dt);

    sim.find_valid_velocity();
    sim.set_invalid_velocity_zero();
    sim.solve_pressure(dt);
}

std::function<void(Simulator& sim, float dt)> get_update_func(const cxxopts::ParseResult& params) {
    std::string update_func_name = params["update"].as<std::string>();
    if (update_func_name == "second_order") {
        return update_second_order;
    } else {
        std::cerr << "invalid update function: " << update_func_name << std::endl;
        std::exit(1);
    }
}

void perform_simulation(Simulator& sim, std::function<void(Simulator&, float)> update,
                        int frame_num, bool use_cfl) {
    Timer::time(
        "simulation",
        [&] {
            sim.init_scene();
            sim.compute_weights();
            sim.find_valid_level_set();
            sim.init_visualizers();
            sim.visualize();
            for (int i = 0; i < frame_num - 1; ++i) {
                if (sim.should_break()) {
                    break;
                }
                try {
                    sim.update(update, 1.0 / FPS, use_cfl);
                } catch (const std::runtime_error& e) {
                    Logger::error(e.what());
                    sim.visualize();
                    break;
                }
                sim.visualize();
            }
            sim.deinit_visualizers();
        },
        WRITE_LOG, TimerMode::HOURS);
}

int main(int argc, const char** argv) {
    cxxopts::Options options("Fluid Simulation");
    options.add_options()         //
        ("h,help", "Print help")  //

        /* output */
        ("o,output", "Output directory path",                                //
         cxxopts::value<std::filesystem::path>()->default_value("default"))  //
        ("frame_num", "",                                                    //
         cxxopts::value<int>()->default_value("400"))                        //
        ("cout", "")                                                         //
        ("d,day", "", cxxopts::value<std::string>()->default_value(""))      //
        ("full_path", "")                                                    //

        /* graphics */
        ("window_height", "", cxxopts::value<int>()->default_value("400"))  //
        ("aspect", "", cxxopts::value<float>()->default_value("1"))         //

        /* simulation */
        ("q,quality", "", cxxopts::value<int>()->default_value("32"))                 //
        ("scene", "", cxxopts::value<std::string>()->default_value("water_drop"))     //
        ("update", "", cxxopts::value<std::string>()->default_value("second_order"))  //

        /* params */
        ("cfl", "negative->not using cfl", cxxopts::value<float>()->default_value("1.0"))  //
        ;
    auto params = options.parse(argc, argv);
    if (params.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    Simulator sim;

    create_output_dest(params, argc, argv);
    set_sim_params(sim, params);
    create_scene_and_camera(sim, params);
    set_visualizers(sim, params);
    perform_simulation(sim, get_update_func(params), params["frame_num"].as<int>(),
                       params["cfl"].as<float>() > 0);
}
