#include <nama/path.h>
#include <nama/utils/utils.h>

#include <fstream>
#include <iostream>
#include <string>

namespace nama::utils {

namespace fs = std::filesystem;

void create_directory(const fs::path& path) {
    if (!fs::create_directory(path)) {
        throw std::runtime_error("Creating directory " + path.string() + " failed!");
    }
}

void overwrite_directoy(const fs::path& path) {
    if (fs::exists(path)) {
        std::string type;
        do {
            std::cerr << "Overwrite " << path << "? [y/n]: ";
            std::cin >> type;
        } while (type != "y" && type != "n");
        if (type == "y") {
            fs::remove_all(path);
        } else {
            std::exit(0);
        }
    }
    fs::create_directories(path);
}

std::string zero_padding(int i, int digits) {
    std::ostringstream sout;
    sout << std::setfill('0') << std::setw(digits) << i;
    return sout.str();
}

std::string get_postfixed_name(const std::string& name, int i, int digits) {
    return name + "_" + zero_padding(i, digits);
}

void create_command_text(int argc, const char** argv, const fs::path& file_path) {
    if (argc) {
        std::string command{};
        for (int i = 0; i < argc; ++i) {
            command += std::string(argv[i]).append(" ");
        }
        std::ofstream file;
        file.open(file_path);
        if (file.is_open()) {
            file << command;
            file.close();
        } else {
            throw std::runtime_error("File '" + file_path.string() + "' not open.");
        }
    }
}

void execute_command(const std::string& command) {
    // std::cout << command << std::endl;
    if (std::system(command.c_str()) != 0) {
        throw std::runtime_error("Command '" + command + "' failed!");
    }
}

void generate_movie(const fs::path& input_dir, const std::string& picture_name,
                    const fs::path& output, unsigned digits_num, const std::string& text) {
    std::string script_path = SCRIPT_DIR / "movie_generator.py";
    script_path += " --indir " + input_dir.string();
    script_path += " --name " + picture_name;
    script_path += " --digits " + std::to_string(digits_num);
    script_path += " --output " + output.string() + ".mp4";
    execute_command("python3 " + script_path);
    if (!text.empty()) {
        script_path = SCRIPT_DIR / "text.py";
        script_path += " --full_path ";
        script_path += " --input " + output.string() + ".mp4";
        script_path += " --text '" + text + "'";
        script_path += " --fontsize " + std::to_string(20);
        script_path += " --output " + output.string() + "_text.mp4";
        execute_command("python3 " + script_path);
    }
}

}  // namespace nama::utils
