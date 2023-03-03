#pragma once

#include <filesystem>

namespace nama::output_config {

void cout_log(bool cout_log);

void set_output_directory(const std::filesystem::path& output_directory,
                          const std::string& day = "");
void set_output_directory_full_path(const std::filesystem::path& output_directory);
void reset_output_directory();

void create_log_file();
void create_command_text(int argc, const char** argv);

}  // namespace nama::output_config
