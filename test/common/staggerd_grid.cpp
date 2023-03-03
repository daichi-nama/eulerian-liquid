#include <gtest/gtest.h>
#include <nama/common/grid.h>
#include <nama/common/staggered_grid.h>

#include <Eigen/Core>

using Eigen::Vector2f;
using Eigen::Vector3f;

using namespace nama;

TEST(StaggeredGrid1, Case1) {
    StaggeredGrid1f grid(16, 0.5, 0, 99);
    grid.create(16, 0.5, 0, 99);
}

TEST(StaggeredGrid2, Case1) {
    StaggeredGrid2f grid(16, 16, 0.5, Vector2f(0, 0), 1);
    grid.create(16, 16, 0.5, Vector2f(0, 0), 1);
    auto [u, v] = grid.get_component();
    ASSERT_EQ(grid.u()(0, 0), 1);
    u(0, 0) = 100;
    ASSERT_EQ(grid.u()(0, 0), 100);
}

TEST(StaggeredGrid3, Case1) {
    StaggeredGrid3f grid(16, 16, 16, 0.5, Vector3f(0, 0, 0), 1);
    grid.create(16, 16, 16, 0.5, Vector3f(0, 0, 0), 1);
    auto [u, v, w] = grid.get_component();
    std::cout << v.slice_i(8) << std::endl;
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
