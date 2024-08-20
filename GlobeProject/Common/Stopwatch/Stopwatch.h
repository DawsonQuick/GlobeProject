#pragma once
#ifndef STOPWATCH_H
#define STOPWATCH_H
#include <iostream>
#include <chrono>
#include <thread>
#include <string>

template<typename Duration>
class Stopwatch {
public:
    void start() {
        start_time_ = std::chrono::high_resolution_clock::now();
        is_running_ = true;
    }

    void stop() {
        if (!is_running_) {
            std::cerr << "Stopwatch has not been started!" << std::endl;
            return;
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<Duration>(end_time - start_time_);
        is_running_ = false;

        std::cout << "Elapsed time: " << elapsed_time.count() << " " << getUnitName<Duration>() << std::endl;
    }
    float stopReturn() {
        if (!is_running_) {
            std::cerr << "Stopwatch has not been started!" << std::endl;
            return 0.0f;
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<Duration>(end_time - start_time_);
        is_running_ = false;

        return elapsed_time.count();
    }

private:
    template<typename T>
    static std::string getUnitName() {
        if constexpr (std::is_same_v<T, std::chrono::nanoseconds>) return "nanoseconds";
        else if constexpr (std::is_same_v<T, std::chrono::microseconds>) return "microseconds";
        else if constexpr (std::is_same_v<T, std::chrono::milliseconds>) return "milliseconds";
        else if constexpr (std::is_same_v<T, std::chrono::seconds>) return "seconds";
        else if constexpr (std::is_same_v<T, std::chrono::minutes>) return "minutes";
        else if constexpr (std::is_same_v<T, std::chrono::hours>) return "hours";
        else return "unknown units";
    }

    std::chrono::high_resolution_clock::time_point start_time_;
    bool is_running_ = false;
};
#endif