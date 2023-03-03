#include <nama/graphics/material.h>

#include <stdexcept>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

namespace nama::graphics {

Material::Material(const glm::vec4& ambient, const glm::vec4& diffuse, const glm::vec4& specular,
                   float shininess)
    : m_ambient(ambient), m_diffuse(diffuse), m_specular(specular), m_shininess(shininess) {}

void Material::set_ambient(float r, float g, float b, float alpha) {
    m_ambient = glm::vec4(r, g, b, alpha);
}

void Material::set_diffuse(float r, float g, float b, float alpha) {
    m_diffuse = glm::vec4(r, g, b, alpha);
}

void Material::set_specular(float r, float g, float b, float alpha) {
    m_specular = glm::vec4(r, g, b, alpha);
}

void Material::set_shininess(float shininess) {
    m_shininess = shininess;
}

const float* Material::get_ambient_ptr() const {
    return glm::value_ptr(m_ambient);
}

const float* Material::get_diffuse_ptr() const {
    return glm::value_ptr(m_diffuse);
}

const float* Material::get_specular_ptr() const {
    return glm::value_ptr(m_specular);
}

float Material::get_shininess() const {
    return m_shininess;
}

void Material::set() const {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, get_ambient_ptr());
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, get_diffuse_ptr());
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, get_specular_ptr());
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, get_shininess());
}

Light::Light(const glm::vec4& position) : m_position(position) {}

void Light::set_position(float x, float y, float z, float w) {
    m_position = glm::vec4(x, y, z, w);
}

const float* Light::get_position_ptr() const {
    return glm::value_ptr(m_position);
}

const float* Light::get_ambient_ptr() const {
    return glm::value_ptr(m_ambient);
}

const float* Light::get_diffuse_ptr() const {
    return glm::value_ptr(m_diffuse);
}

const float* Light::get_specular_ptr() const {
    return glm::value_ptr(m_specular);
}

void MaterialSet::add_light(const Light& l) {
    if (m_lights.size() < 8) {
        m_lights.push_back(l);
    } else {
        throw std::runtime_error("too much lights");
    }
}

void MaterialSet::add_material(const Material& m) {
    m_materials.push_back(m);
}

void MaterialSet::enable_lights() const {
    for (size_t i = 0; i < m_lights.size(); ++i) {
        enable_light_at(i);
    }
}

void MaterialSet::enable_light_at(int i) const {
    int light_enum = 0;
    switch (i) {
        case 0:
            light_enum = GL_LIGHT0;
            break;
        case 1:
            light_enum = GL_LIGHT1;
            break;
        case 2:
            light_enum = GL_LIGHT2;
            break;
        case 3:
            light_enum = GL_LIGHT3;
            break;
        case 4:
            light_enum = GL_LIGHT4;
            break;
        case 5:
            light_enum = GL_LIGHT5;
            break;
        case 6:
            light_enum = GL_LIGHT6;
            break;
        case 7:
            light_enum = GL_LIGHT7;
            break;
        default:
            throw std::invalid_argument("invalid light index");
            break;
    }
    glEnable(light_enum);
    glLightfv(light_enum, GL_POSITION, m_lights.at(i).get_position_ptr());
    glLightfv(light_enum, GL_AMBIENT, m_lights.at(i).get_ambient_ptr());
    glLightfv(light_enum, GL_DIFFUSE, m_lights.at(i).get_diffuse_ptr());
    glLightfv(light_enum, GL_SPECULAR, m_lights.at(i).get_specular_ptr());
}

void MaterialSet::set_material(int i) const {
    if (i < 0) {
        throw std::invalid_argument("negative index in 'set_material'");
    }
    if (i >= static_cast<int>(m_materials.size())) {
        throw std::invalid_argument("too large index in 'set_material'");
    }
    m_materials.at(i).set();
}

}  // namespace nama::graphics
