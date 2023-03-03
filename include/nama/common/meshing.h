#pragma once

#include <functional>

#include "constants.h"
#include "grid.h"
#include "mesh.h"

namespace nama {

template <typename T, typename TV>
inline TV vertex_interp(const TV& pos0, const TV& pos1, T levelset0, T levelset1, T isolevel) {
    assert(levelset0 != levelset1);
    T mu   = -(levelset0 - isolevel) / (levelset1 - levelset0);
    TV pos = pos0 + mu * (pos1 - pos0);
    return pos;
}

template <typename T>
void set_height_mesh(const Grid2<T>& grid, Mesh3f& mesh, T coeff = static_cast<T>(1)) {
    if (mesh.empty()) {
        mesh = get_height_mesh(grid, coeff);
    } else {
        update_height_mesh(grid, mesh, coeff);
    }
}

template <typename T>
Mesh3f get_height_mesh(const Grid2<T>& grid, T coeff = static_cast<T>(1)) {
    Mesh3f result(MeshType::QUADS);
    serial_loop<int>({0, grid.ni() - 1}, {0, grid.nj() - 1}, [&](int i, int j) {
        result.add_index(grid.calc_index(i + 0, j + 0));
        result.add_index(grid.calc_index(i + 1, j + 0));
        result.add_index(grid.calc_index(i + 1, j + 1));
        result.add_index(grid.calc_index(i + 0, j + 1));
    });
    serial_loop(grid, [&](int i, int j) {
        typename Grid2<T>::vector_type pos = grid.pos(i, j);
        result.add_vertex(Mesh3f::vector_type(pos[0], grid(i, j) * coeff, pos[1]));
    });
    return result;
}

template <typename T>
void update_height_mesh(const Grid2<T>& grid, Mesh3f& mesh, T coeff = static_cast<T>(1)) {
    assert(!mesh.empty());
    auto& vertices = mesh.vertices();
    vertices.resize(grid.size());
    serial_loop(grid, [&](int i, int j) {
        typename Grid2<T>::vector_type pos = grid.pos(i, j);
        vertices[grid.calc_index(i, j)] = Mesh3f::vector_type(pos[0], grid(i, j) * coeff, pos[1]);
    });
}

template <typename T>
inline static int edge_to_index(const Grid3<T>& grid, int i, int j, int k, int table_id) {
    auto calc_index = [](int ni, int nj, [[maybe_unused]] int nk, int i_, int j_, int k_) -> int {
        return i_ + ni * (j_ + nj * k_);
    };
    int ni    = grid.ni() + 1;
    int nj    = grid.nj() + 1;
    int nk    = grid.nk() + 1;
    int x_max = (ni - 1) * nj * nk;
    int y_max = x_max + ni * (nj - 1) * nk;
    switch (table_id) {
        // x
        case 0:
            return calc_index(ni - 1, nj, nk, i, j, k);
        case 2:
            return calc_index(ni - 1, nj, nk, i, j + 1, k);
        case 4:
            return calc_index(ni - 1, nj, nk, i, j, k + 1);
        case 6:
            return calc_index(ni - 1, nj, nk, i, j + 1, k + 1);
        // y
        case 3:
            return calc_index(ni, nj - 1, nk, i, j, k) + x_max;
        case 1:
            return calc_index(ni, nj - 1, nk, i + 1, j, k) + x_max;
        case 7:
            return calc_index(ni, nj - 1, nk, i, j, k + 1) + x_max;
        case 5:
            return calc_index(ni, nj - 1, nk, i + 1, j, k + 1) + x_max;
        // z
        case 8:
            return calc_index(ni, nj, nk - 1, i, j, k) + y_max;
        case 9:
            return calc_index(ni, nj, nk - 1, i + 1, j, k) + y_max;
        case 11:
            return calc_index(ni, nj, nk - 1, i, j + 1, k) + y_max;
        case 10:
            return calc_index(ni, nj, nk - 1, i + 1, j + 1, k) + y_max;
        default:
            throw std::invalid_argument("invalid table_id");
    }

    return 0;
}

enum class MeshingMethod {
    MARCHING_CUBES,
    DUAL_MARCHING_CUBES,
};

template <typename T>
Mesh3f get_mesh(
    const Grid3<T>& grid, T isolevel, MeshingMethod method,
    std::function<bool(int, int, int)> condition = [](int, int, int) { return true; });

}  // namespace nama
