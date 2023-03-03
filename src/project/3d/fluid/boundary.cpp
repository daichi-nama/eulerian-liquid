#include <nama/math/level_set.h>
#include <nama/project/3d/fluid/boundary.h>

namespace nama::_3d::fluid {

void compute_staggered_grid_weight(StaggeredGrid3f& weight, const Grid3f& nodal_solid_level_set) {
    auto& u_weight = weight.u();
    auto& v_weight = weight.v();
    auto& w_weight = weight.w();
    parallel_loop(u_weight, [&](int i, int j, int k) {
        u_weight(i, j, k) = 1
                            - math::fraction_inside(nodal_solid_level_set(i, j, k),
                                                    nodal_solid_level_set(i, j + 1, k),
                                                    nodal_solid_level_set(i, j, k + 1),
                                                    nodal_solid_level_set(i, j + 1, k + 1));
        u_weight(i, j, k) = std::clamp(u_weight(i, j, k), 0.0f, 1.0f);
    });
    parallel_loop(v_weight, [&](int i, int j, int k) {
        v_weight(i, j, k) = 1
                            - math::fraction_inside(nodal_solid_level_set(i, j, k),
                                                    nodal_solid_level_set(i, j, k + 1),
                                                    nodal_solid_level_set(i + 1, j, k),
                                                    nodal_solid_level_set(i + 1, j, k + 1));
        v_weight(i, j, k) = std::clamp(v_weight(i, j, k), 0.0f, 1.0f);
    });
    parallel_loop(w_weight, [&](int i, int j, int k) {
        w_weight(i, j, k) = 1
                            - math::fraction_inside(nodal_solid_level_set(i, j, k),
                                                    nodal_solid_level_set(i, j + 1, k),
                                                    nodal_solid_level_set(i + 1, j, k),
                                                    nodal_solid_level_set(i + 1, j + 1, k));
        w_weight(i, j, k) = std::clamp(w_weight(i, j, k), 0.0f, 1.0f);
    });
}

}  // namespace nama::_3d::fluid
