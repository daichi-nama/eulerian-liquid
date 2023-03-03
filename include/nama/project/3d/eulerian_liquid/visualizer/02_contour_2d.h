#pragma once

#include "visualizer_base.h"

namespace nama::_3d::eulerian_liquid {

class ContourVisualizer2d : public VisualizerBase, public OpenglVisualizer {
   public:
    ContourVisualizer2d(Simulator* sim, const std::vector<std::string>& image_names,
                        const std::vector<std::string>& file_names, const std::string& window_name,
                        int window_height, int watching_index);

    virtual void enable_capabilities() const override;
    virtual void init_texture() override;

    virtual void visualize() override;

   private:
    GLuint m_texture_id;
    int m_watching_index;
};

}  // namespace nama::_3d::eulerian_liquid
