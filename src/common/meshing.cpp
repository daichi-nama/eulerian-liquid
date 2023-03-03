#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wshadow"
#include <dualmc/dualmc.h>
#pragma GCC diagnostic pop

#include <nama/common/meshing.h>

namespace nama {

template <typename T>
Mesh3f get_mesh(const Grid3<T>& grid, T isolevel, MeshingMethod method,
                std::function<bool(int, int, int)> condition) {
    using namespace internal;
    using value_type  = typename Grid3<T>::value_type;
    using vector_type = typename Grid3<T>::vector_type;
    static_assert(std::is_same<vector_type, Mesh3f::vector_type>::value);
    Mesh3f result;

    if (method == MeshingMethod::MARCHING_CUBES) {
        result.set_type(MeshType::TRIANGLES);
        auto meshing_func = [&](int i, int j, int k) {
            bool should_calc = condition(i, j, k) && condition(i + 1, j, k)
                               && condition(i + 1, j + 1, k) && condition(i, j + 1, k)
                               && condition(i, j, k + 1) && condition(i + 1, j, k + 1)
                               && condition(i + 1, j + 1, k + 1) && condition(i, j + 1, k + 1);
            if (!should_calc) {
                return;
            }
            std::unordered_map<int, int> vert_map;
            std::array<vector_type, 12> vert_list;
            std::array<value_type, 8> vert_level_set;
            std::array<std::tuple<int, int, int>, 8> query = {
                std::make_tuple(i + 0, j + 0, k + 0),  //
                std::make_tuple(i + 1, j + 0, k + 0),  //
                std::make_tuple(i + 1, j + 1, k + 0),  //
                std::make_tuple(i + 0, j + 1, k + 0),  //
                std::make_tuple(i + 0, j + 0, k + 1),  //
                std::make_tuple(i + 1, j + 0, k + 1),  //
                std::make_tuple(i + 1, j + 1, k + 1),  //
                std::make_tuple(i + 0, j + 1, k + 1),  //
            };
            int cube_id = 0;
            for (int n = 0; n < 8; ++n) {
                const auto& [x, y, z] = query[n];
                vert_list[n]          = grid.pos(x, y, z);
                vert_level_set[n]     = grid(x, y, z);
                if (vert_level_set[n] < isolevel) {
                    cube_id |= (1 << n);
                }
            }
            int edge_id = EDGE_TABLE3[cube_id];
            if (edge_id == 0) {
                return;
            }
            auto vert_list_temp = vert_list;
            if (edge_id & 1)
                vert_list[0] = vertex_interp(vert_list_temp[0], vert_list_temp[1],
                                             vert_level_set[0], vert_level_set[1], isolevel);
            if (edge_id & 2)
                vert_list[1] = vertex_interp(vert_list_temp[1], vert_list_temp[2],
                                             vert_level_set[1], vert_level_set[2], isolevel);
            if (edge_id & 4)
                vert_list[2] = vertex_interp(vert_list_temp[2], vert_list_temp[3],
                                             vert_level_set[2], vert_level_set[3], isolevel);
            if (edge_id & 8)
                vert_list[3] = vertex_interp(vert_list_temp[3], vert_list_temp[0],
                                             vert_level_set[3], vert_level_set[0], isolevel);
            if (edge_id & 16)
                vert_list[4] = vertex_interp(vert_list_temp[4], vert_list_temp[5],
                                             vert_level_set[4], vert_level_set[5], isolevel);
            if (edge_id & 32)
                vert_list[5] = vertex_interp(vert_list_temp[5], vert_list_temp[6],
                                             vert_level_set[5], vert_level_set[6], isolevel);
            if (edge_id & 64)
                vert_list[6] = vertex_interp(vert_list_temp[6], vert_list_temp[7],
                                             vert_level_set[6], vert_level_set[7], isolevel);
            if (edge_id & 128)
                vert_list[7] = vertex_interp(vert_list_temp[7], vert_list_temp[4],
                                             vert_level_set[7], vert_level_set[4], isolevel);
            if (edge_id & 256)
                vert_list[8] = vertex_interp(vert_list_temp[0], vert_list_temp[4],
                                             vert_level_set[0], vert_level_set[4], isolevel);
            if (edge_id & 512)
                vert_list[9] = vertex_interp(vert_list_temp[1], vert_list_temp[5],
                                             vert_level_set[1], vert_level_set[5], isolevel);
            if (edge_id & 1024)
                vert_list[10] = vertex_interp(vert_list_temp[2], vert_list_temp[6],
                                              vert_level_set[2], vert_level_set[6], isolevel);
            if (edge_id & 2048)
                vert_list[11] = vertex_interp(vert_list_temp[3], vert_list_temp[7],
                                              vert_level_set[3], vert_level_set[7], isolevel);

            for (int c = 0; TRI_TABLE3[cube_id][c] != -1; c += 3) {
                std::array<int, 3> idx_arr;
                for (auto p : {0, 1, 2}) {
                    int table_id = TRI_TABLE3[cube_id][c + p];
                    int pos_idx  = edge_to_index(grid, i, j, k, table_id);
                    if (vert_map.count(pos_idx) == 0) {
                        result.add_vertex(vert_list[table_id]);
                        int idx_          = static_cast<int>(result.vertices().size()) - 1;
                        vert_map[pos_idx] = idx_;
                    }
                    idx_arr[p] = vert_map[pos_idx];
                }
                result.add_index(idx_arr);
            }
        };
        serial_loop<int>({0, grid.ni() - 1}, {0, grid.nj() - 1}, {0, grid.nk() - 1}, meshing_func);
    } else if (method == MeshingMethod::DUAL_MARCHING_CUBES) {
        result.set_type(MeshType::QUADS);
        dualmc::DualMC<T> dmc;
        std::vector<dualmc::Vertex> vertices;
        std::vector<dualmc::Quad> quads;
        std::vector<T> data(grid.size());
        std::copy(grid.cbegin(), grid.cend(), data.begin());
        dmc.build(data.data(), grid.ni(), grid.nj(), grid.nk(), isolevel, true, false, vertices,
                  quads);
        for (const auto& v : vertices) {
            result.add_vertex(vector_type(v.x, v.y, v.z) * grid.dx() + grid.origin());
        }
        for (auto const& q : quads) {
            result.add_index(q.i0, q.i3, q.i2, q.i1);
        }
    }

    return result;
}

template Mesh3f get_mesh<float>(const Grid3<float>& grid, float isolevel, MeshingMethod method,
                                std::function<bool(int, int, int)> condition);
template Mesh3f get_mesh<double>(const Grid3<double>& grid, double isolevel, MeshingMethod method,
                                 std::function<bool(int, int, int)> condition);

}  // namespace nama
