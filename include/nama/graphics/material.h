#pragma once

#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace nama::graphics {

class Material {
   public:
    Material() = default;
    Material(const glm::vec4 &ambient, const glm::vec4 &diffuse, const glm::vec4 &specular,
             float shininess);

    void set_ambient(float r, float g, float b, float alpha);
    void set_diffuse(float r, float g, float b, float alpha);
    void set_specular(float r, float g, float b, float alpha);
    void set_shininess(float shininess);

    const float *get_ambient_ptr() const;
    const float *get_diffuse_ptr() const;
    const float *get_specular_ptr() const;
    float get_shininess() const;

    void set() const;

   private:
    glm::vec4 m_ambient;
    glm::vec4 m_diffuse;
    glm::vec4 m_specular;
    float m_shininess = 0;
};

class Light {
   public:
    Light() = default;
    explicit Light(const glm::vec4 &position);

    void set_position(float x, float y, float z, float w);

    const float *get_position_ptr() const;
    const float *get_ambient_ptr() const;
    const float *get_diffuse_ptr() const;
    const float *get_specular_ptr() const;

   private:
    glm::vec4 m_position;

    glm::vec4 m_ambient  = glm::vec4(0, 0, 0, 1);
    glm::vec4 m_diffuse  = glm::vec4(1, 1, 1, 1);
    glm::vec4 m_specular = glm::vec4(1, 1, 1, 1);
};

class MaterialSet {
   public:
    void add_light(const Light &l);
    void add_material(const Material &m);

    void enable_lights() const;

    void set_material(int i) const;

   private:
    void enable_light_at(int i) const;

    std::vector<Material> m_materials;
    std::vector<Light> m_lights;
};

}  // namespace nama::graphics
