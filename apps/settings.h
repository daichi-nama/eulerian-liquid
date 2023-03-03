#pragma once

#include <nama/output_config.h>

#include "cxxopts.hpp"

inline static constexpr int FPS = 60;

inline void create_output_dest(const cxxopts::ParseResult& params, int argc, const char** argv) {
    using namespace nama::output_config;

    if (params.count("full_path")) {
        set_output_directory_full_path(params["output"].as<std::filesystem::path>());
    } else {
        set_output_directory(params["output"].as<std::filesystem::path>(),
                             params["day"].as<std::string>());
    }
    reset_output_directory();
    create_log_file();
    cout_log(params.count("cout"));
    create_command_text(argc, argv);
}
