#include <nama/graphics/camera.h>

#include <iostream>

namespace nama::graphics {

CameraBase::CameraBase(float aspect) : m_aspect(aspect) {}

float CameraBase::get_aspect() const {
    return m_aspect;
}

CameraOrtho::CameraOrtho(const glm::vec2& min_bound, const glm::vec2& max_bound)
    : m_min_bound(min_bound), m_max_bound(max_bound) {
    check_bound();
    calc_aspect();
}

void CameraOrtho::set_plane(const std::string& plane) {
    if (plane == "xy") {
        m_plane = Plane::XY;
    } else if (plane == "xz") {
        m_plane = Plane::XZ;
    } else if (plane == "zy") {
        m_plane = Plane::ZY;
    } else if (plane == "none") {
        m_plane = Plane::ZY;
    } else {
        throw std::invalid_argument("invalid plane in 'CameraOrtho::set_plane'");
    }
}

void CameraOrtho::set_bound(const glm::vec2& min_bound, const glm::vec2& max_bound) {
    m_min_bound = min_bound;
    m_max_bound = max_bound;
    check_bound();
    calc_aspect();
}

void CameraOrtho::set_min_bound(float x, float y) {
    m_min_bound = glm::vec2(x, y);
}

void CameraOrtho::set_max_bound(float x, float y) {
    m_max_bound = glm::vec2(x, y);
}

glm::vec2 CameraOrtho::get_min_bound() const {
    return m_min_bound;
}

glm::vec2 CameraOrtho::get_max_bound() const {
    return m_max_bound;
}

void CameraOrtho::check_bound() {
    if (m_max_bound.x <= m_min_bound.x) {
        throw std::invalid_argument("invalid length_x");
    }
    if (m_max_bound.y <= m_min_bound.y) {
        throw std::invalid_argument("invalid length_y");
    }
}

void CameraOrtho::calc_aspect() {
    m_aspect = (m_max_bound.x - m_min_bound.x) / (m_max_bound.y - m_min_bound.y);
}

void CameraOrtho::set() const {
    constexpr float DEPTH = 100;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (m_plane == Plane::NONE) {
        glOrtho(m_min_bound.x, m_max_bound.x, m_min_bound.y, m_max_bound.y, -1.0, 1.0);
    } else {
        glOrtho(m_min_bound.x, m_max_bound.x, m_min_bound.y, m_max_bound.y, -DEPTH, DEPTH);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (m_plane == Plane::XZ) {
        gluLookAt(0, -1, 0,  // eye
                  0, 0, 0,   // center
                  0, 0, 1);  // up
    } else if (m_plane == Plane::ZY) {
        gluLookAt(-1, 0, 0,  // eye
                  0, 0, 0,   // center
                  0, 1, 0);  // up
    }
}

CameraPerspective::CameraPerspective(float aspect) : CameraBase(aspect) {}

CameraPerspective::CameraPerspective(float aspect, const glm::vec3& eye, const glm::vec3& center,
                                     const glm::vec3& up)
    : CameraBase(aspect), m_eye(eye), m_center(center), m_up(up) {}

void CameraPerspective::set_eye(float x, float y, float z) {
    m_eye = glm::vec3(x, y, z);
}

void CameraPerspective::set_center(float x, float y, float z) {
    m_center = glm::vec3(x, y, z);
}

void CameraPerspective::set_up(float x, float y, float z) {
    m_up = glm::vec3(x, y, z);
}

void CameraPerspective::set_fov(float fov) {
    m_fov = fov;
}

void CameraPerspective::set_near(float near) {
    m_near = near;
}

void CameraPerspective::set_far(float far) {
    m_far = far;
}

void CameraPerspective::set_aspect(float aspect) {
    m_aspect = aspect;
}

glm::vec3 CameraPerspective::get_eye() const {
    return m_eye;
}

glm::vec3 CameraPerspective::get_center() const {
    return m_center;
}

glm::vec3 CameraPerspective::get_up() const {
    return m_up;
}

float CameraPerspective::get_fov() const {
    return m_fov;
}

float CameraPerspective::get_near() const {
    return m_near;
}

float CameraPerspective::get_far() const {
    return m_far;
}

void CameraPerspective::set() const {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(m_fov, m_aspect, m_near, m_far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(m_eye.x, m_eye.y, m_eye.z,           // eye
              m_center.x, m_center.y, m_center.z,  // center
              m_up.x, m_up.y, m_up.z);             // up
}

CameraOrtho create_camera_for_2d_visualization(const glm::vec3& min_bound,
                                               const glm::vec3& max_bound, const std::string& plane,
                                               float margin) {
    CameraOrtho result;

    if (plane == "xy") {
        result.set_min_bound(min_bound.x - margin, min_bound.y - margin);
        result.set_max_bound(max_bound.x + margin, max_bound.y + margin);
    } else if (plane == "xz") {
        result.set_min_bound(min_bound.x - margin, min_bound.z - margin);
        result.set_max_bound(max_bound.x + margin, max_bound.z + margin);
    } else if (plane == "zy") {
        result.set_min_bound(min_bound.z - margin, min_bound.y - margin);
        result.set_max_bound(max_bound.z + margin, max_bound.y + margin);
    } else {
        std::cout << "invalid plane: " << plane << std::endl;
        throw std::invalid_argument("invalid plane in 'create_camera_for_2d_visualization'");
    }
    result.set_plane(plane);
    result.calc_aspect();
    return result;
}

}  // namespace nama::graphics
