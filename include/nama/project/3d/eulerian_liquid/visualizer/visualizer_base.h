#pragma once

#include <nama/abstract_visualizer.h>
#include <nama/opengl_visualizer.h>

#include "../simulator.h"

namespace nama::_3d::eulerian_liquid {

class Simulator;

class VisualizerBase {
   public:
    VisualizerBase(Simulator* sim);
    virtual ~VisualizerBase() = default;

   protected:
    Simulator* m_sim = nullptr;
};

}  // namespace nama::_3d::eulerian_liquid
