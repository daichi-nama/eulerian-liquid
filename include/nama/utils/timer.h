#pragma once

#include <nama/utils/logger.h>
#include <nama/utils/writer.h>

#include <chrono>

namespace nama::utils {

enum class TimerMode {
    NANOSEC,
    MICROSEC,
    MILLISEC,
    SEC,
    MINUTES,
    HOURS,
};

class Timer {
   public:
    Timer() = delete;

    template <typename F>
    static double time(const std::string& item_name, F f, unsigned char flag = WRITE_LOG,
                       TimerMode mode = TimerMode::SEC) {
        auto start = std::chrono::high_resolution_clock::now();
        f();
        auto end    = std::chrono::high_resolution_clock::now();
        double time = get_time(end - start, mode);
        if (flag & WRITE_CSV) {
            Writer::append(item_name, time);
        }
        if (flag & WRITE_LOG) {
            Logger::info(item_name + " " + unit(mode), time);
        }
        return time;
    }

   private:
    template <typename T>
    static double get_time(T elapse, TimerMode mode) {
        switch (mode) {
            case TimerMode::NANOSEC:
                return std::chrono::duration_cast<std::chrono::nanoseconds>(elapse).count();
                break;
            case TimerMode::MICROSEC:
                return std::chrono::duration_cast<std::chrono::microseconds>(elapse).count();
                break;
            case TimerMode::MILLISEC:
                return std::chrono::duration_cast<std::chrono::microseconds>(elapse).count() / 1e3;
                break;
            case TimerMode::SEC:
                return std::chrono::duration_cast<std::chrono::microseconds>(elapse).count() / 1e6;
                break;
            case TimerMode::MINUTES:
                return std::chrono::duration_cast<std::chrono::minutes>(elapse).count();
                break;
            case TimerMode::HOURS:
                return std::chrono::duration_cast<std::chrono::minutes>(elapse).count() / 60.0;
                break;
            default:
                throw std::runtime_error("invalid time mode");
                break;
        }
    }

    static std::string unit(TimerMode mode) {
        switch (mode) {
            case TimerMode::NANOSEC:
                return "(ns)";
                break;
            case TimerMode::MICROSEC:
                return "(mu s)";
                break;
            case TimerMode::MILLISEC:
                return "(ms)";
            case TimerMode::SEC:
                return "(s)";
            case TimerMode::MINUTES:
                return "(m)";
            case TimerMode::HOURS:
                return "(h)";
            default:
                throw std::runtime_error("invalid time mode");
                break;
        }
    }
};

}  // namespace nama::utils
