#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace nama {

class AbstractVisualizer {
   public:
    AbstractVisualizer(const std::vector<std::string>& image_names,
                       const std::vector<std::string>& file_names);
    virtual ~AbstractVisualizer() = default;

    virtual void init();
    virtual void visualize() = 0;

    virtual bool should_close() const;

    static void set_output_directory(const std::filesystem::path& output);

    void create_result_directories() const;

    void increment();
    void generate_movie() const;

   protected:
    int get_image_output_number() const;
    int get_file_output_number() const;
    std::filesystem::path get_image_directory(int i) const;
    std::filesystem::path get_file_directory(int i) const;
    std::filesystem::path get_image_path(int i, const std::string& extention) const;
    std::filesystem::path get_file_path(int i, const std::string& extention,
                                        bool sequence = true) const;

    inline static std::filesystem::path s_output;

    int m_frame_count = 0;

   private:
    std::vector<std::string> m_image_names;
    std::vector<std::string> m_file_names;

    inline static constexpr int DIGITS_NUM = 4;
};

}  // namespace nama
