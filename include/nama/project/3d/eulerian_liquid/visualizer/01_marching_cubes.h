#pragma once

#include "visualizer_base.h"

namespace nama::_3d::eulerian_liquid {

class MarchingCubesVisualizer : public VisualizerBase, public OpenglVisualizer {
   public:
    MarchingCubesVisualizer(Simulator* sim, const std::vector<std::string>& image_names,
                            const std::vector<std::string>& file_names,
                            const std::string& window_name, int window_height,
                            const graphics::MaterialSet& material_set,
                            std::unique_ptr<graphics::CameraBase>&& camera_ptr);

    virtual void enable_capabilities() const override;

    virtual void visualize() override;
};

}  // namespace nama::_3d::eulerian_liquid
