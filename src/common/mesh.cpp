#include <nama/common/mesh.h>
#include <tiny_obj_loader.h>

#include <fstream>
#include <iostream>

namespace nama {

template <typename TV, typename I>
void Mesh<TV, I>::load(const std::filesystem::path& mesh_path) {
    clear();

    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(mesh_path)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        throw std::runtime_error("mesh loading failed");
    }
    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();

    for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
        m_vertices.emplace_back(attrib.vertices[i + 0],  //
                                attrib.vertices[i + 1],  //
                                attrib.vertices[i + 2]);
    }

    size_t first_fv;
    for (size_t s = 0; s < shapes.size(); ++s) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
            first_fv  = fv;
            m_type    = int_to_mesh_type(first_fv);
            if (fv != first_fv) {
                throw std::runtime_error("Mixed mesh is not supported");
            }
            for (size_t v = 0; v < fv; ++v) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                m_indices.push_back(idx.vertex_index);
            }
            index_offset += fv;
        }
    }
}

template <typename TV, typename I>
void Mesh<TV, I>::save(const std::filesystem::path& output_path) const {
    if (output_path.extension() == ".obj") {
        std::ofstream file(output_path);
        if (!file) {
            throw std::runtime_error("File '" + output_path.string() + "' not open.");
        }

        const size_t offset = static_cast<size_t>(get_type());
        for (auto const& v : m_vertices) {
            file << "v " << v(0) << ' ' << v(1) << ' ' << v(2) << std::endl;
        }
        for (size_t i = 0; i < m_indices.size(); i += offset) {
            file << "f ";
            for (size_t j = 0; j < offset; ++j) {
                file << m_indices[i + j] + static_cast<size_t>(1);
                if (j != offset - static_cast<size_t>(1)) {
                    file << ' ';
                } else {
                    file << std::endl;
                }
            }
        }
        file.close();
    } else {
        throw std::invalid_argument("unsupported extension");
    }
}

template class Mesh<Eigen::Vector3f, int>;
template class Mesh<Eigen::Vector3d, int>;

}  // namespace  nama
