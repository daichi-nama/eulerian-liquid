#pragma once

#include "interpolation.h"

namespace nama {

template <typename StaggeredGrid, typename TV>
class StaggeredGridField {
   public:
    explicit StaggeredGridField(const StaggeredGrid& grid) : m_grid(grid) {}

    TV operator()(const TV& x) const {
        return math::lerp_staggered<TV>(x, m_grid);
    }

   private:
    const StaggeredGrid& m_grid;
};

}  // namespace nama

namespace nama::math {

template <typename TV, typename T, typename VelocityField>
TV trace_rk1(const TV& pos, T dt, const VelocityField& field) {
    TV velocity = field(pos);
    return pos + dt * velocity;
}

template <typename TV, typename T, typename VelocityField>
TV trace_rk2(const TV& pos, T dt, const VelocityField& field) {
    TV velocity = field(pos);
    velocity    = field(pos + static_cast<T>(0.5) * dt * velocity);
    return pos + dt * velocity;
}

}  // namespace nama::math
