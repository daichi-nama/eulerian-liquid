#include <nama/project/3d/eulerian_liquid/scene.h>

namespace nama::_3d::eulerian_liquid {

using namespace fluid;

void Scene::add_liquid_emitter(std::shared_ptr<LevelSetEmitterBase>&& emitter) {
    m_liquid_emitter_list.add_emitter(std::move(emitter));
}

void Scene::add_solid_emitter(std::shared_ptr<LevelSetEmitterBase>&& emitter) {
    m_solid_emitter_list.add_emitter(std::move(emitter));
}

void Scene::apply_liquid_emitter(Grid3f& grid) const {
    m_liquid_emitter_list.apply(grid);
}

void Scene::apply_solid_emitter(Grid3f& grid) const {
    m_solid_emitter_list.apply(grid);
}

}  // namespace nama::_3d::eulerian_liquid
