#include <gtest/gtest.h>
#include <nama/common/grid.h>
#include <nama/math/interpolation.h>

using Eigen::Vector2d;
using Eigen::Vector2f;
using Eigen::Vector3d;
using Eigen::Vector3f;

using namespace nama;
using namespace nama::math;

TEST(IndexAndFraction, Case1) {
    Grid1f grid(10, 0.1, 0);
    float x      = 0;
    auto [i, fx] = calc_index_and_fraction(x, grid.origin(), grid.dx());
    ASSERT_EQ(i, -1);
    ASSERT_FLOAT_EQ(fx, 0.5);

    x               = 0.16;
    std::tie(i, fx) = calc_index_and_fraction(x, grid.origin(), grid.dx());
    ASSERT_EQ(i, 1);
    ASSERT_NEAR(fx, 0.1, grid.dx() * 0.01);
}

TEST(Grid1InderpolationTest, Case1) {
    Grid1f grid(10, 0.1, 0);
    ASSERT_FLOAT_EQ(lerp(0.5f, grid), 0);
    ASSERT_FLOAT_EQ(weno6_interp(0.5f, grid), 0);
}

TEST(Grid2InderpolationTest, Case1) {
    Grid2f grid(10, 10, 0.1, Vector2f(0, 0));
    ASSERT_FLOAT_EQ(lerp(Vector2f(0.5, 0.5), grid), 0);
    ASSERT_FLOAT_EQ(weno6_interp(Vector2f(0.5, 0.5), grid), 0);
}

TEST(Grid3InderpolationTest, Case1) {
    Grid3f grid(10, 10, 10, 0.1, Vector3f(0, 0, 0));
    ASSERT_FLOAT_EQ(lerp(Vector3f(0.5, 0.5, 0.5), grid), 0);
    ASSERT_FLOAT_EQ(weno6_interp(Vector3f(0.5, 0.5, 0.5), grid), 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
