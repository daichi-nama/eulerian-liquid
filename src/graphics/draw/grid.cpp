#include <nama/common/constants.h>
#include <nama/common/meshing.h>
#include <nama/graphics/draw/grid.h>

namespace nama::graphics {

using Eigen::Vector2f;

void draw_mesh(const Grid2f& grid, float isolevel, bool only_contour) {
    using namespace internal;
    if (only_contour) {
        glBegin(GL_LINES);
    } else {
        glBegin(GL_TRIANGLES);
    }
    serial_loop<int>({0, grid.ni() - 1}, {0, grid.nj() - 1}, [&](int i, int j) {
        std::array<Vector2f, 8> vert_list;
        std::array<float, 4> vert_level_set;
        std::array<std::pair<int, int>, 4> query = {
            std::make_pair(i + 0, j + 0),
            std::make_pair(i + 1, j + 0),
            std::make_pair(i + 1, j + 1),
            std::make_pair(i + 0, j + 1),
        };
        int square_id = 0;
        for (int n = 0; n < 4; ++n) {
            auto [x, y]       = query[n];
            vert_list[n]      = grid.pos(x, y);
            vert_level_set[n] = grid(x, y);
            if (vert_level_set[n] < isolevel) {
                square_id |= (1 << n);
            }
        }
        if (int edge_id = EDGE_TABLE2[square_id]) {
            if (edge_id & 1) {
                vert_list[4] = vertex_interp(vert_list[0], vert_list[1], vert_level_set[0],
                                             vert_level_set[1], isolevel);
            }
            if (edge_id & 2) {
                vert_list[5] = vertex_interp(vert_list[1], vert_list[2], vert_level_set[1],
                                             vert_level_set[2], isolevel);
            }
            if (edge_id & 4) {
                vert_list[6] = vertex_interp(vert_list[2], vert_list[3], vert_level_set[2],
                                             vert_level_set[3], isolevel);
            }
            if (edge_id & 8) {
                vert_list[7] = vertex_interp(vert_list[3], vert_list[0], vert_level_set[3],
                                             vert_level_set[0], isolevel);
            }
        }

        if (only_contour) {
            for (int c = 0; LINE_TABLE2[square_id][c] != -1; ++c) {
                int table_id = LINE_TABLE2[square_id][c];
                glVertex2fv(vert_list[table_id].data());
            }
        } else {
            for (int c = 0; TRI_TABLE2[square_id][c] != -1; c += 3) {
                for (int p = 0; p < 3; p++) {
                    int table_id = TRI_TABLE2[square_id][c + p];
                    glVertex2fv(vert_list[table_id].data());
                }
            }
        }
    });
    glEnd();
}

}  // namespace nama::graphics
