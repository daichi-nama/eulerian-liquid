#include <nama/graphics/draw/tools.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

namespace nama::graphics {

void draw_one_velocity(const Eigen::Vector2f& x, const Eigen::Vector2f& vel, bool arrowhead) {
    constexpr float arrow_scale = 0.02;
    constexpr float arrow_size  = 0.012;
    if (vel.norm() > 1e-4) {
        glPushMatrix();
        {
            Eigen::Vector2f vel_normalized = vel.normalized();
            float cos_theta                = vel_normalized.dot(Eigen::Vector2f(0, 1));
            float theta{std::acos(cos_theta)};
            if (vel(0) > 0) {
                theta = -theta;
            }
            float theta_degree = theta * 180 / M_PI;
            glTranslatef(x(0), x(1), 0.0);
            glRotatef(theta_degree, 0.0, 0.0, 1.0);

            glBegin(GL_LINES);
            glVertex2f(0.0, 0.0);
            glVertex2f(0.0, vel.norm() * arrow_scale);
            glEnd();

            if (arrowhead) {
                glBegin(GL_TRIANGLES);
                glVertex2f(0.0, vel.norm() * arrow_scale + arrow_size);
                glVertex2f(arrow_size / 2, vel.norm() * arrow_scale);
                glVertex2f(-arrow_size / 2, vel.norm() * arrow_scale);
                glEnd();
            }
        }
        glPopMatrix();
    }
}
}  // namespace nama::graphics
