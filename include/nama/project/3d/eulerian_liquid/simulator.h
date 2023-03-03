#pragma once

#include <nama/common/grid.h>
#include <nama/common/staggered_grid.h>
#include <nama/simulator_base.h>

#include <vector>

#include "scene.h"
#include "simulation_params.h"

namespace nama::_3d::eulerian_liquid {

class VisualizerBase;

class Simulator : public SimulatorBase {
   public:
    Simulator() = default;

    void update(std::function<void(Simulator&, float)> f, float dt_frame, bool use_cfl);

    // settings
    void set_params(const SimulationParams& params);
    void set_scene(Scene& scene);

    // initialization
    void create_grid(float width, int ni, int nj, int nk);
    void init_scene();

    /* simulation */
    // advection
    void advect_velocity(float dt);
    void advect_level_set(float dt);

    // force
    void add_gravity(float dt);

    // projection
    void solve_pressure(float dt);

    // extrapolation
    void extrapolate_level_set();
    void extrapolate_velocity();
    void find_valid_velocity();
    void find_valid_level_set();
    void set_invalid_velocity_zero();

    // reinitialization
    void reinitialize();

    // boundary
    void compute_weights();
    void calc_cell_centered_solid_phi();

   public:
    SimulationParams m_params;
    Scene m_scene;

    float m_dx;
    int m_ni, m_nj, m_nk;

    StaggeredGrid3f m_velocity, m_velocity_temp;
    StaggeredGrid3c m_velocity_valid;
    StaggeredGrid3f m_velocity_weight;

    Grid3f m_level_set, m_level_set_temp;
    Grid3c m_level_set_valid;
    Grid3i m_indices;
    Grid3f m_pressure;

    Grid3f m_nodal_solid_phi;
    Grid3f m_cell_centered_solid_level_set;
};

}  // namespace nama::_3d::eulerian_liquid
