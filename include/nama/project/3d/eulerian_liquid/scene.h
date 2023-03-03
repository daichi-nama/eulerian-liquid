#pragma once

#include <nama/project/3d/fluid/levelset_emitter.h>

#include <Eigen/Core>
#include <memory>
#include <vector>

namespace nama::_3d::eulerian_liquid {

class Scene {
   public:
    void add_liquid_emitter(std::shared_ptr<fluid::LevelSetEmitterBase>&& emitter);
    void add_solid_emitter(std::shared_ptr<fluid::LevelSetEmitterBase>&& emitter);

    void apply_liquid_emitter(Grid3f& grid) const;
    void apply_solid_emitter(Grid3f& grid) const;

   private:
    fluid::LevelSetEmitterList m_liquid_emitter_list;
    fluid::LevelSetEmitterList m_solid_emitter_list;
};

}  // namespace nama::_3d::eulerian_liquid
