#pragma once

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <string>

namespace nama::utils {

enum WRITE_MODE {
    WRITE_NOTHING = 0,
    WRITE_LOG     = 1 << 0,
    WRITE_CSV     = 1 << 1,
};

class Logger {
    enum LogLevel {
        DEBUG = 0,
        INFO  = 1,
        WARN  = 2,
        ERROR = 3,
    };

    inline static constexpr LogLevel LEVEL = DEBUG;

   public:
    static void set_output_file(const std::filesystem::path& output) {
        s_output = output;
    }

    static void create_output_file() {
        if (s_file_out) {
            s_file_out.close();
        }
        s_file_out.open(s_output);
        check_s_file_out();
        if (std::atexit(close) != 0) {
            throw std::runtime_error("std::atexit(close) failed!");
        }
    }

    static void set_cout(bool is_cout) {
        s_is_cout = is_cout;
    }

    static void close() {
        s_file_out.close();
    }

    static void debug(const std::string& message) {
        if constexpr (LEVEL == DEBUG) {
            check_s_file_out();
            s_file_out << "[DEBUG] " << message << std::endl;
            if (s_is_cout) {
                std::cout << "[DEBUG] " << message << std::endl;
            }
        }
    }

    template <typename T>
    static void debug(const std::string& item_name, T value) {
        if constexpr (LEVEL == DEBUG) {
            check_s_file_out();
            s_file_out << "[DEBUG] " << std::left << std::setw(WIDTH) << item_name + ":" << value
                       << std::endl;
            if (s_is_cout) {
                std::cout << "[DEBUG] " << std::left << std::setw(WIDTH) << item_name + ":" << value
                          << std::endl;
            }
        }
    }

    static void info(const std::string& message) {
        if constexpr (LEVEL <= INFO) {
            check_s_file_out();
            s_file_out << "[INFO]  " << message << std::endl;
            if (s_is_cout) {
                std::cout << "[INFO]  " << message << std::endl;
            }
        }
    }

    template <typename T>
    static void info(const std::string& item_name, T value) {
        if constexpr (LEVEL <= INFO) {
            check_s_file_out();
            s_file_out << "[INFO]  " << std::left << std::setw(WIDTH) << item_name + ":" << value
                       << std::endl;
            if (s_is_cout) {
                std::cout << "[INFO]  " << std::left << std::setw(WIDTH) << item_name + ":" << value
                          << std::endl;
            }
        }
    }

    static void warn(const std::string& message) {
        if constexpr (LEVEL <= WARN) {
            check_s_file_out();
            s_file_out << "[WARN]  " << message << std::endl;
            std::cerr << "[WARN]  " << message << std::endl;
        }
    }

    template <typename T>
    static void warn(const std::string& item_name, T value) {
        if constexpr (LEVEL <= WARN) {
            check_s_file_out();
            s_file_out << "[WARN]  " << std::left << std::setw(WIDTH) << item_name + ":" << value
                       << std::endl;
            if (s_is_cout) {
                std::cout << "[WARN]  " << std::left << std::setw(WIDTH) << item_name + ":" << value
                          << std::endl;
            }
        }
    }

    static void error(const std::string& message) {
        if constexpr (LEVEL <= ERROR) {
            check_s_file_out();
            s_file_out << "[ERROR] " << message << std::endl;
            std::cerr << "[ERROR] " << message << std::endl;
        }
    }

    template <typename T>
    static void error(const std::string& item_name, T value) {
        if constexpr (LEVEL <= ERROR) {
            check_s_file_out();
            s_file_out << "[ERROR] " << std::left << std::setw(WIDTH) << item_name + ":" << value
                       << std::endl;
            if (s_is_cout) {
                std::cout << "[ERROR] " << std::left << std::setw(WIDTH) << item_name + ":" << value
                          << std::endl;
            }
        }
    }

    static void new_line() {
        check_s_file_out();
        s_file_out << std::endl;
        if (s_is_cout) {
            std::cout << std::endl;
        }
    }

   private:
    static void check_s_file_out() {
        if (!s_file_out) {
            throw std::runtime_error("unable to open a file for the log");
        }
    }

    inline static constexpr int WIDTH            = 36;
    inline static bool s_is_cout                 = false;
    inline static std::filesystem::path s_output = "log.txt";
    inline static std::ofstream s_file_out;
};

}  // namespace nama::utils
