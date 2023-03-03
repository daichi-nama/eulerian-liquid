#pragma once

#include <stdexcept>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace nama::graphics {

class CameraBase {
   public:
    CameraBase() = default;
    CameraBase(float aspect);

    virtual ~CameraBase() = default;

    float get_aspect() const;

    virtual void set() const = 0;

   protected:
    float m_aspect = 45.0;  // width / height
};

class CameraOrtho : public CameraBase {
    enum class Plane {
        NONE,
        XY,
        XZ,
        ZY,
    };

   public:
    CameraOrtho() = default;
    CameraOrtho(const glm::vec2& min_bound, const glm::vec2& max_bound);

    virtual ~CameraOrtho() = default;

    void set_plane(const std::string& plane);

    void set_bound(const glm::vec2& min_bound, const glm::vec2& max_bound);
    void set_min_bound(float x, float y);
    void set_max_bound(float x, float y);

    glm::vec2 get_min_bound() const;
    glm::vec2 get_max_bound() const;

    void check_bound();

    void calc_aspect();

    virtual void set() const override;

   private:
    glm::vec2 m_min_bound, m_max_bound;
    Plane m_plane = Plane::NONE;
};

class CameraPerspective : public CameraBase {
   public:
    CameraPerspective() = default;
    explicit CameraPerspective(float aspect);
    CameraPerspective(float aspect, const glm::vec3& eye, const glm::vec3& center,
                      const glm::vec3& up);

    virtual ~CameraPerspective() = default;

    void set_eye(float x, float y, float z);
    void set_center(float x, float y, float z);
    void set_up(float x, float y, float z);
    void set_fov(float fov);
    void set_near(float near);
    void set_far(float far);
    void set_aspect(float aspect);

    glm::vec3 get_eye() const;
    glm::vec3 get_center() const;
    glm::vec3 get_up() const;
    float get_fov() const;
    float get_near() const;
    float get_far() const;

    virtual void set() const override;

   private:
    glm::vec3 m_eye, m_center, m_up;
    float m_fov = 45, m_near = 0.1, m_far = 1000;
};

CameraOrtho create_camera_for_2d_visualization(const glm::vec3& min_bound,
                                               const glm::vec3& max_bound, const std::string& plane,
                                               float margin);

}  // namespace nama::graphics
