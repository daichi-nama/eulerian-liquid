#pragma once

#include <nama/abstract_visualizer.h>
#include <nama/graphics/camera.h>
#include <nama/graphics/draw.h>
#include <nama/graphics/init.h>
#include <nama/graphics/material.h>
#include <nama/graphics/window.h>
#include <nama/path.h>

#include <vector>

namespace nama {

class OpenglVisualizer : public AbstractVisualizer {
   public:
    OpenglVisualizer(const std::vector<std::string>& image_names,
                     const std::vector<std::string>& file_names);
    OpenglVisualizer(const std::vector<std::string>& image_names,
                     const std::vector<std::string>& file_names, const std::string& window_name,
                     int window_height, std::unique_ptr<graphics::CameraBase>&& camera_ptr);
    virtual ~OpenglVisualizer() = default;

    virtual void init() override;

    void create_window();
    virtual void init_texture();
    virtual void set_camera() const;
    virtual void enable_capabilities() const = 0;
    virtual void init_lights() const;

    virtual bool should_close() const final override;

    virtual std::string get_window_name() const;
    virtual void set_window_name(const std::string& text);

    void create_window_name_text(int i) const;

   protected:
    std::unique_ptr<graphics::CameraBase> m_camera_ptr;
    graphics::Window m_window;

    graphics::MaterialSet m_material_set;
};

}  // namespace nama
