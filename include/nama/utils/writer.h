#pragma once

#include <nama/utils/logger.h>
#include <nama/utils/macros.h>
#include <nama/utils/utils.h>
#include <nama/utils/writer.h>

namespace nama::utils {
class Writer {
   public:
    static void set_output_directory(const std::filesystem::path& out_dir) {
        s_out_dir = out_dir;
    }
    static void create_output_directory() {
        utils::create_directory(s_out_dir);
    }

    template <typename T>
    static void append(const std::string& file_name, T val, unsigned char flag = WRITE_NOTHING) {
        std::ofstream ofs(s_out_dir / (file_name + ".csv"), std::ios::app);
        if (!ofs) {
            throw std::runtime_error("File '" + (s_out_dir / (file_name + ".csv")).string()
                                     + "' not open.");
        }
        ofs << val << std::endl;
        if (flag & WRITE_LOG) {
            Logger::info(file_name, val);
        }
    }
    template <typename T, typename U>
    static void append(const std::string& file_name, T val0, U val1) {
        std::ofstream ofs(s_out_dir / (file_name + ".csv"), std::ios::app);
        if (!ofs) {
            throw std::runtime_error("File '" + (s_out_dir / (file_name + ".csv")).string()
                                     + "' not open.");
        }
        ofs << val0 << "," << val1 << std::endl;
    }

   private:
    inline static std::filesystem::path s_out_dir;
};

}  // namespace nama::utils
