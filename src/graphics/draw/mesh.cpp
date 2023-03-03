#include <nama/graphics/draw/mesh.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <Eigen/Core>

namespace nama::graphics {

using Eigen::Vector3f;

void draw_mesh(Mesh3f& mesh) {
    size_t vert_num = static_cast<size_t>(mesh.get_type());
    mesh.calc_face_normals();
    const auto& indices      = mesh.indices();
    const auto& vertices     = mesh.vertices();
    const auto& face_normals = mesh.face_normals();
    if (mesh.get_type() == MeshType::TRIANGLES) {
        glBegin(GL_TRIANGLES);
        for (size_t i = 0; i < indices.size(); i += vert_num) {
            const Vector3f& normal = face_normals[i / vert_num];
            const Vector3f& p0     = vertices[indices[i + 0]];
            const Vector3f& p1     = vertices[indices[i + 1]];
            const Vector3f& p2     = vertices[indices[i + 2]];
            glNormal3fv(normal.data());
            glVertex3fv(p0.data());
            glVertex3fv(p1.data());
            glVertex3fv(p2.data());
        }
        glEnd();
    } else if (mesh.get_type() == MeshType::QUADS) {
        glBegin(GL_QUADS);
        for (size_t i = 0; i < indices.size(); i += vert_num) {
            const Vector3f& normal = face_normals[i / vert_num];
            const Vector3f& p0     = vertices[indices[i + 0]];
            const Vector3f& p1     = vertices[indices[i + 1]];
            const Vector3f& p2     = vertices[indices[i + 2]];
            const Vector3f& p3     = vertices[indices[i + 3]];
            glNormal3fv(normal.data());
            glVertex3fv(p0.data());
            glVertex3fv(p1.data());
            glVertex3fv(p2.data());
            glVertex3fv(p3.data());
        }
        glEnd();
    } else {
        throw std::runtime_error("unsupported mesh type");
    }
}

}  // namespace nama::graphics
