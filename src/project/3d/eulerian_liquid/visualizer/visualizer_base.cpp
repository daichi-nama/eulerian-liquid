#include <nama/project/3d/eulerian_liquid/simulator.h>
#include <nama/project/3d/eulerian_liquid/visualizer/visualizer_base.h>

namespace nama::_3d::eulerian_liquid {

VisualizerBase::VisualizerBase(Simulator* sim) : m_sim(sim) {}

}  // namespace nama::_3d::eulerian_liquid
