#include <nama/constants.h>
#include <nama/math/level_set.h>
#include <nama/math/ode.h>
#include <nama/project/3d/eulerian_liquid/simulator.h>
#include <nama/project/3d/fluid/advection.h>
#include <nama/project/3d/fluid/boundary.h>
#include <nama/project/3d/fluid/extrapolation.h>
#include <nama/project/3d/fluid/levelset_emitter.h>
#include <nama/project/3d/fluid/projection.h>
#include <nama/project/3d/fluid/reinitialization.h>
#include <nama/project/3d/fluid/time_stepping.h>
#include <nama/utils/logger.h>
#include <nama/utils/macros.h>

#include <Eigen/IterativeLinearSolvers>
#include <Eigen/SparseCore>

namespace nama::_3d::eulerian_liquid {

using Eigen::Vector3f;
using namespace utils;

void Simulator::update(std::function<void(Simulator&, float)> f, float dt_frame, bool use_cfl) {
    if (use_cfl) {
        float cfl      = m_params.cfl;
        float dt_total = 0;
        // int count      = 1;
        while (dt_total < dt_frame) {
            float dt_sub = fluid::calc_time_step(cfl, m_velocity);
            if (dt_total + dt_sub > dt_frame) {
                dt_sub = dt_frame - dt_total;
            }
            Logger::info("substep size (%)", int(100 * (dt_total + dt_sub) / dt_frame));
            f(*this, dt_sub);
            dt_total += dt_sub;
            // ++count;
            utils::Logger::new_line();
        }
    } else {
        f(*this, dt_frame);
    }
}

void Simulator::set_params(const SimulationParams& params) {
    m_params = params;
}

void Simulator::set_scene(Scene& scene) {
    m_scene = std::move(scene);
}

void Simulator::create_grid(float width, int ni, int nj, int nk) {
    m_ni = ni;
    m_nj = nj;
    m_nk = nk;
    m_dx = width / ni;

    m_velocity.create(m_ni, m_nj, m_nk, m_dx, Eigen::Vector3f(0, 0, 0));
    m_velocity_valid.create(m_ni, m_nj, m_nk, m_dx, Eigen::Vector3f(0, 0, 0), 1);
    m_velocity_weight.create(m_ni, m_nj, m_nk, m_dx, Eigen::Vector3f(0, 0, 0));

    m_level_set.create(m_ni, m_nj, m_nk, m_dx, Vector3f(0, 0, 0));
    m_level_set_valid.create(m_ni, m_nj, m_nk, m_dx, Vector3f(0, 0, 0), 1);
    m_indices.create(m_ni, m_nj, m_nk, m_dx, Vector3f(0, 0, 0));
    m_pressure.create(m_ni, m_nj, m_nk, m_dx, Vector3f(0, 0, 0));

    m_nodal_solid_phi.create(m_ni + 1, m_nj + 1, m_nk + 1, m_dx,
                             Vector3f(-m_dx * 0.5, -m_dx * 0.5, -m_dx * 0.5));
    m_cell_centered_solid_level_set.create(m_ni, m_nj, m_nk, m_dx, Vector3f(0, 0, 0));
}

void Simulator::init_scene() {
    m_scene.apply_liquid_emitter(m_level_set);
    m_scene.apply_solid_emitter(m_nodal_solid_phi);
    calc_cell_centered_solid_phi();
}

// advection
void Simulator::advect_velocity(float dt) {
    m_velocity_temp = m_velocity;
    fluid::advect_staggered_grid(m_velocity, m_velocity_temp, m_velocity_temp, dt);
}

void Simulator::advect_level_set(float dt) {
    m_level_set_temp = m_level_set;
    fluid::advect_collocated_grid(m_level_set, m_level_set_temp, m_velocity, dt);
}

// force
void Simulator::add_gravity(float dt) {
    auto& v = m_velocity.v();
    parallel_loop(v, [&](int i, int j, int k) { v(i, j, k) -= constants::gravity<float> * dt; });
}

// projection
void Simulator::solve_pressure(float dt) {
    int mat_size = fluid::assign_indices(m_indices, m_level_set, m_velocity_weight);
    Eigen::SparseMatrix<double, Eigen::RowMajor> A;
    Eigen::VectorXd d;
    fluid::build_matrix(A, d, mat_size, m_indices, m_level_set, m_velocity, m_velocity_weight, dt);

    Eigen::ConjugateGradient<Eigen::SparseMatrix<double, Eigen::RowMajor>, Eigen::Lower,
                             Eigen::IncompleteCholesky<double>>
        solver;
    solver.setMaxIterations(1000);
    solver.setTolerance(1e-4);
    solver.compute(A);

    Eigen::VectorXd p = solver.solve(d);
    fluid::build_grid_from_vector(m_pressure, p, m_indices, 0);
    fluid::update_velocity_using_pressure(m_velocity, m_pressure, m_level_set, dt);
}

// extrapolation
void Simulator::extrapolate_level_set() {
    fluid::extrapolate_const(m_level_set, m_level_set_valid, 10);
}

void Simulator::extrapolate_velocity() {
    fluid::extrapolate(m_velocity.u(), m_velocity_valid.u(), 10);
    fluid::extrapolate(m_velocity.v(), m_velocity_valid.v(), 10);
    fluid::extrapolate(m_velocity.w(), m_velocity_valid.w(), 10);
}

void Simulator::find_valid_velocity() {
    for (int d = 0; d < m_velocity_valid.dim; ++d) {
        std::fill(m_velocity_valid[d].begin(), m_velocity_valid[d].end(), 1);
        parallel_loop(m_velocity_valid[d], [&](int i, int j, int k) {
            if (m_velocity_weight[d](i, j, k) == 0) {
                m_velocity_valid[d](i, j, k) = 0;
            }
        });
    }
    auto& u_valid = m_velocity_valid.u();
    auto& v_valid = m_velocity_valid.v();
    auto& w_valid = m_velocity_valid.w();
    parallel_loop<int>  //
        ({1, u_valid.ni() - 1}, {1, u_valid.nj() - 1}, {1, u_valid.nk() - 1},
         [&](int i, int j, int k) {
             if (m_level_set(i - 1, j, k) >= 0 && m_level_set(i, j, k) >= 0) {
                 u_valid(i, j, k) = 0;
             }
         });
    parallel_loop<int>  //
        ({1, v_valid.ni() - 1}, {1, v_valid.nj() - 1}, {1, v_valid.nk() - 1},
         [&](int i, int j, int k) {
             if (m_level_set(i, j - 1, k) >= 0 && m_level_set(i, j, k) >= 0) {
                 v_valid(i, j, k) = 0;
             }
         });
    parallel_loop<int>  //
        ({1, w_valid.ni() - 1}, {1, w_valid.nj() - 1}, {1, w_valid.nk() - 1},
         [&](int i, int j, int k) {
             if (m_level_set(i, j, k - 1) >= 0 && m_level_set(i, j, k) >= 0) {
                 w_valid(i, j, k) = 0;
             }
         });
}

void Simulator::find_valid_level_set() {
    std::fill(m_level_set_valid.begin(), m_level_set_valid.end(), 1);
    parallel_loop(m_level_set_valid, [&](int i, int j, int k) {
        if (m_cell_centered_solid_level_set(i, j, k) < 0) {
            m_level_set_valid(i, j, k) = 0;
        }
    });
}

void Simulator::set_invalid_velocity_zero() {
    for (int d = 0; d < m_velocity.dim; ++d) {
#pragma omp parallel for
        for (size_t i = 0; i < m_velocity_valid[d].size(); ++i) {
            if (m_velocity_valid[d][i] == 0) {
                m_velocity[d][i] = 0;
            }
        }
    }
}

// reinitialization
void Simulator::reinitialize() {
    fluid::reinitialize_fast_marching(m_level_set, m_level_set_temp, m_level_set_valid);
}

// boundary

void Simulator::compute_weights() {
    fluid::compute_staggered_grid_weight(m_velocity_weight, m_nodal_solid_phi);
}

void Simulator::calc_cell_centered_solid_phi() {
    parallel_loop(m_cell_centered_solid_level_set, [&](int i, int j, int k) {
        m_cell_centered_solid_level_set(i, j, k)
            = (m_nodal_solid_phi(i, j, k) + m_nodal_solid_phi(i + 1, j, k)
               + m_nodal_solid_phi(i, j + 1, k) + m_nodal_solid_phi(i + 1, j + 1, k)
               + m_nodal_solid_phi(i, j, k + 1) + m_nodal_solid_phi(i + 1, j, k + 1)
               + m_nodal_solid_phi(i, j + 1, k + 1) + m_nodal_solid_phi(i + 1, j + 1, k + 1))
              * 0.125;
    });
}

}  // namespace nama::_3d::eulerian_liquid
