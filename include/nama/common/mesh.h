#pragma once

#include <nama/math/geometry.h>

#include <Eigen/Geometry>
#include <algorithm>
#include <array>
#include <filesystem>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace nama {

enum class MeshType : unsigned {
    LINES     = 2u,
    TRIANGLES = 3u,
    QUADS     = 4u,
};

template <typename TV, typename I>
class Mesh {
   public:
    using vector_type = TV;
    using index_type  = I;

    Mesh() = default;

    explicit Mesh(MeshType type) : m_type(type) {}

    Mesh(MeshType type, const std::vector<vector_type>& vertices,
         const std::vector<index_type>& indices)
        : m_type(type), m_vertices(vertices), m_indices(indices) {}

    void clear() {
        m_vertices.clear();
        m_indices.clear();
        m_face_normals.clear();
    }

    void add_vertex(const vector_type& v) {
        m_vertices.push_back(v);
    }

    void add_vertex(vector_type&& v) {
        m_vertices.push_back(std::move(v));
    }

    template <typename... Args>
    void add_index(Args... args) {
        (m_indices.push_back(args), ...);
    }

    template <typename T, size_t N>
    void add_index(const std::array<T, N>& container) {
        for (const index_type e : container) {
            m_indices.push_back(e);
        }
    }

    template <typename T>
    void add_index(const std::vector<T>& container) {
        for (const index_type e : container) {
            m_indices.push_back(e);
        }
    }

    MeshType get_type() const {
        return m_type;
    }

    void set_type(MeshType type) {
        m_type = type;
    }

    const vector_type& vertex(index_type i) const {
        assert(i >= 0 && i < static_cast<index_type>(m_vertices.size()));
        return m_vertices[i];
    }

    vector_type& vertex(index_type i) {
        assert(i >= 0 && i < static_cast<index_type>(m_vertices.size()));
        return m_vertices[i];
    }

    const index_type& indices(index_type i) const {
        assert(i >= 0 && i < static_cast<index_type>(m_indices.size()));
        return m_indices[i];
    }

    index_type& indices(index_type i) {
        assert(i >= 0 && i < static_cast<index_type>(m_indices.size()));
        return m_indices[i];
    }

    const vector_type& face_normals(index_type i) const {
        assert(i >= 0 && i < static_cast<index_type>(m_face_normals.size()));
        return m_face_normals[i];
    }

    vector_type& face_normals(index_type i) {
        assert(i >= 0 && i < static_cast<index_type>(m_face_normals.size()));
        return m_face_normals[i];
    }

    const std::vector<vector_type>& vertices() const {
        return m_vertices;
    }

    std::vector<vector_type>& vertices() {
        return m_vertices;
    }

    const std::vector<index_type>& indices() const {
        return m_indices;
    }

    std::vector<index_type>& indices() {
        return m_indices;
    }

    const std::vector<vector_type>& face_normals() const {
        return m_face_normals;
    }

    std::vector<vector_type>& face_normals() {
        return m_face_normals;
    }

    bool empty() const {
        return !m_vertices.size() || !m_indices.size();
    }

    void calc_face_normals() {
        assert(m_type != MeshType::LINES);
        size_t vert_num = static_cast<size_t>(m_type);
        m_face_normals.resize(m_indices.size() / vert_num);
#pragma omp parallel for
        for (size_t i = 0; i < m_indices.size(); i += vert_num) {
            const vector_type& p0        = m_vertices[m_indices[i + 0]];
            const vector_type& p1        = m_vertices[m_indices[i + 1]];
            const vector_type& p2        = m_vertices[m_indices[i + 2]];
            m_face_normals[i / vert_num] = math::calc_normal(p0, p1, p2);
        }
    }

    // geometry

    template <typename T>
    void scale(T s) {
        for (vector_type& v : m_vertices) {
            v *= s;
        }
    }

    void translate(const vector_type& t) {
        for (vector_type& v : m_vertices) {
            v += t;
        }
    }

    void get_bound(vector_type& min_bound, vector_type& max_bound) {
        // TODO:
        min_bound = vector_type(+1000, +1000, +1000);
        max_bound = vector_type(-1000, -1000, -1000);
        for (vector_type& v : m_vertices) {
            for (int i = 0; i < 3; ++i) {
                min_bound[i] = std::min(min_bound[i], v[i]);
                max_bound[i] = std::max(max_bound[i], v[i]);
            }
        }
    }

    // io
    void load(const std::filesystem::path& mesh_path);
    void save(const std::filesystem::path& output_path) const;

   private:
    static MeshType int_to_mesh_type(int face_vert_num) {
        switch (face_vert_num) {
            case 3:
                return MeshType::TRIANGLES;
                break;
            case 4:
                return MeshType::QUADS;
                break;
            default:
                throw std::runtime_error("unsupported mesh type");
                break;
        }
    }

   private:
    MeshType m_type;
    std::vector<vector_type> m_vertices;
    std::vector<index_type> m_indices;
    std::vector<vector_type> m_face_normals;
};

// using Mesh2f = Mesh<Eigen::Vector2f, int>;
// using Mesh2d = Mesh<Eigen::Vector2d, int>;
using Mesh3f = Mesh<Eigen::Vector3f, int>;
using Mesh3d = Mesh<Eigen::Vector3d, int>;

}  // namespace nama
