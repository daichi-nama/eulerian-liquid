#include <gtest/gtest.h>
#include <nama/common/grid.h>
#include <nama/common/meshing.h>

#include <Eigen/Geometry>

using Eigen::Vector3f;

using namespace nama;

template <typename T, typename TV>
static auto sphere(const TV& center, T radius) {
    return [=](const Vector3f& x) { return (x - center).norm() - radius; };
}

TEST(MarchingCubes, Case1) {
    Grid3f grid(64, 64, 64, 1.0 / 64, Vector3f(0, 0, 0));
    Vector3f center(0.5, 0.5, 0.5);
    float radius = 0.3;
    grid.set(sphere(center, radius));
    auto mesh = get_mesh(grid, 0.0f, MeshingMethod::MARCHING_CUBES);
    mesh.save("marhing_cubes.obj");
    mesh = get_mesh(grid, 0.0f, MeshingMethod::DUAL_MARCHING_CUBES);
    mesh.save("dual_marhing_cubes.obj");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
