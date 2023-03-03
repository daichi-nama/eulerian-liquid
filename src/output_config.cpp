#include <nama/abstract_visualizer.h>
#include <nama/output_config.h>
#include <nama/path.h>
#include <nama/utils/logger.h>
#include <nama/utils/utils.h>
#include <nama/utils/writer.h>

namespace nama::output_config {

namespace fs = std::filesystem;

static fs::path g_output_directory = "";

void cout_log(bool cout_log) {
    utils::Logger::set_cout(cout_log);
}

void set_output_directory(const std::filesystem::path& output_directory, const std::string& day) {
    if (day.empty()) {
        g_output_directory = OUT_DIR / output_directory;
    } else {
        g_output_directory = ARCHIVE_DIR / day / output_directory;
    }

    AbstractVisualizer::set_output_directory(g_output_directory);
    utils::Logger::set_output_file(g_output_directory / "02_log.txt");
    utils::Writer::set_output_directory(g_output_directory / "data");
}

void set_output_directory_full_path(const std::filesystem::path& output_directory) {
    g_output_directory = output_directory;

    AbstractVisualizer::set_output_directory(g_output_directory);
    utils::Logger::set_output_file(g_output_directory / "02_log.txt");
    utils::Writer::set_output_directory(g_output_directory / "data");
}

void reset_output_directory() {
    if (g_output_directory.empty()) {
        throw std::runtime_error("The output directory is not set.");
    }
    utils::overwrite_directoy(g_output_directory);
    utils::Writer::create_output_directory();
}

void create_log_file() {
    utils::Logger::create_output_file();
}

void create_command_text(int argc, const char** argv) {
    if (g_output_directory.empty()) {
        throw std::runtime_error("The output directory is not set.");
    }
    fs::path output = g_output_directory / "01_command.txt";
    utils::create_command_text(argc, argv, output);
}

}  // namespace nama::output_config
