#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace nama::utils {

void create_directory(const std::filesystem::path& path);
void overwrite_directoy(const std::filesystem::path& path);

std::string zero_padding(int i, int digits);
std::string get_postfixed_name(const std::string& name, int i, int digits);

void create_command_text(int argc, const char** argv, const std::filesystem::path& file_path);

void execute_command(const std::string& command);
void generate_movie(const std::filesystem::path& input_dir, const std::string& picture_name,
                    const std::filesystem::path& output, unsigned digits_num,
                    const std::string& text = "");

}  // namespace nama::utils
