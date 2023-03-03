#pragma once

#include <nama/abstract_visualizer.h>

#include <memory>
#include <vector>

namespace nama {

class SimulatorBase {
   public:
    SimulatorBase()  = default;
    ~SimulatorBase() = default;

    // basic visualization interface
    void create_result_directories() const;

    void init_visualizers() const;
    void visualize();
    void deinit_visualizers() const;
    bool should_break() const;

    void add_visualizer(std::unique_ptr<AbstractVisualizer>&& vis);

   protected:
    int m_frame_count = 0;

   private:
    std::vector<std::unique_ptr<AbstractVisualizer>> m_vis_arr;
};

}  // namespace nama
