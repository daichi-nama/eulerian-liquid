#include <nama/abstract_visualizer.h>
#include <nama/utils/utils.h>

namespace nama {

namespace fs = std::filesystem;

AbstractVisualizer::AbstractVisualizer(const std::vector<std::string>& image_names,
                                       const std::vector<std::string>& file_names)
    : m_image_names(image_names), m_file_names(file_names) {}

void AbstractVisualizer::init() {}

bool AbstractVisualizer::should_close() const {
    return false;
}

void AbstractVisualizer::set_output_directory(const fs::path& output) {
    s_output = output;
}

void AbstractVisualizer::create_result_directories() const {
    for (const auto& e : m_image_names) {
        if (!e.empty()) {
            utils::create_directory(s_output / e);
        }
    }
    for (const auto& e : m_file_names) {
        if (!e.empty()) {
            utils::create_directory(s_output / e);
        }
    }
}

void AbstractVisualizer::increment() {
    ++m_frame_count;
}

void AbstractVisualizer::generate_movie() const {
    for (const auto& e : m_image_names) {
        std::cout << "Creating " << e << ".mp4 ..." << std::endl;
        utils::generate_movie(s_output / e, e, s_output / e, DIGITS_NUM);
    }
}

int AbstractVisualizer::get_image_output_number() const {
    return m_image_names.size();
}

int AbstractVisualizer::get_file_output_number() const {
    return m_file_names.size();
}

fs::path AbstractVisualizer::get_image_directory(int i) const {
    if (i < 0 || i >= static_cast<int>(m_image_names.size())) {
        throw std::out_of_range("'out of range' in 'get_image_directory'");
    }
    return s_output / m_image_names.at(i);
}

fs::path AbstractVisualizer::get_file_directory(int i) const {
    if (i < 0 || i >= static_cast<int>(m_file_names.size())) {
        throw std::out_of_range("'out of range' in 'get_file_directory'");
    }
    return s_output / m_file_names.at(i);
}

fs::path AbstractVisualizer::get_image_path(int i, const std::string& extention) const {
    if (i < 0 || i >= static_cast<int>(m_image_names.size())) {
        throw std::out_of_range("'out of range' in 'image_name'");
    }
    std::string name = utils::get_postfixed_name(m_image_names.at(i), m_frame_count, DIGITS_NUM);
    return (s_output / m_image_names.at(i) / name).replace_extension(extention);
}

fs::path AbstractVisualizer::get_file_path(int i, const std::string& extention,
                                           bool sequence) const {
    if (i < -1 || i >= static_cast<int>(m_file_names.size())) {
        throw std::out_of_range("'out of range' in 'file_name'");
    }
    std::string name
        = sequence ? utils::get_postfixed_name(m_file_names.at(i), m_frame_count, DIGITS_NUM)
                   : m_file_names.at(i);
    return (s_output / m_file_names.at(i) / name).replace_extension(extention);
}

}  // namespace nama
