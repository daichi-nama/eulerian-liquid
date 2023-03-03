#include <nama/project/3d/fluid/extrapolation.h>

namespace nama::_3d::fluid {

void extrapolate(Array3f& grid, Array3c& valid, int layers) {
    Array3f grid_temp = grid;
    Array3c old_valid;
    auto is_valid = [&](int x, int y, int z, float& sum, int& count) -> bool {
        if (x < 0 || x >= valid.ni() || y < 0 || y >= valid.nj() || z < 0 || z >= valid.nk()) {
            return false;
        }
        if (old_valid(x, y, z)) {
            sum += grid(x, y, z);
            ++count;
            return true;
        }
        return false;
    };
    for (int l = 0; l < layers; ++l) {
        bool should_return = true;
        old_valid          = valid;
        parallel_loop(grid, [&](int i, int j, int k) {
            float sum = 0;
            int count = 0;
            if (!old_valid(i, j, k)) {
#pragma omp atomic write
                should_return = false;
                for (int kk = -1; kk <= 1; ++kk) {
                    for (int jj = -1; jj <= 1; ++jj) {
                        for (int ii = -1; ii <= 1; ++ii) {
                            is_valid(i + ii, j + jj, k + kk, sum, count);
                        }
                    }
                }
                if (count > 0) {
                    grid_temp(i, j, k) = sum / static_cast<float>(count);
                    valid(i, j, k)     = 1;
                }
            }
        });
        if (should_return) {
            return;
        }
        grid = grid_temp;
    }
}

void extrapolate_const(Array3f& grid, const Array3c& valid, int layers) {
    auto valid_copy = valid;
    extrapolate(grid, valid_copy, layers);
}

}  // namespace nama::_3d::fluid
