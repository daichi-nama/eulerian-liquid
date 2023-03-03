#include <gtest/gtest.h>
#include <nama/common/grid.h>

using namespace nama;

TEST(Array1Test, Case1) {
    Array1f arr;
    ASSERT_EQ(arr.size(), 0);
    ASSERT_EQ(arr.ni(), 0);
    ASSERT_EQ(arr.size(), 0);
    ASSERT_ANY_THROW(arr.resize(-1, 10));
}

TEST(Array1Test, Case2) {
    Array1f arr0(10);
    serial_loop(arr0, [&](int i) { ASSERT_EQ(arr0[i], 0.0); });
    serial_loop(arr0, [&](int i) { ASSERT_EQ(arr0(i), 0.0); });

    Array1f arr1(10, 1.0);
    serial_loop(arr1, [&](int i) { ASSERT_EQ(arr1[i], 1.0); });
    serial_loop(arr1, [&](int i) { ASSERT_EQ(arr1(i), 1.0); });
    arr1[1] = 2.0;
    ASSERT_EQ(arr1[1], 2.0);

    ASSERT_EQ(arr1.size(), 10);
    ASSERT_EQ(arr1.ni(), 10);
#ifndef NDEBUG
    ASSERT_DEATH(arr1(-1), "");
#endif
}

TEST(Grid1Test, Case1) {
    Grid1f grid(10, 0.1, 0.05, 10);
    grid(0) = -10;
    ASSERT_FLOAT_EQ(grid.dx(), 0.1);
    ASSERT_FLOAT_EQ(grid.pos(0), 0.05 + 0.1 * 0.5);
    serial_loop(grid, [&](int i) { ASSERT_FLOAT_EQ(grid.extrapolate(i), 10); });
    ASSERT_FLOAT_EQ(grid.extrapolate(-1), -10);
}

TEST(Grid1Test, Case2) {
    Grid1f grid(10, 0.1, 0);
    for (auto& g : grid) {
        g = 100;
    }
    serial_loop(grid, [&](int i) { ASSERT_FLOAT_EQ(grid(i), 100); });
    std::fill(grid.begin(), grid.end(), 3);
    serial_loop(grid, [&](int i) { ASSERT_FLOAT_EQ(grid(i), 3); });
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
