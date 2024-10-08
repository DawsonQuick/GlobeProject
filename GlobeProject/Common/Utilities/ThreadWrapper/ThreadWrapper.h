#pragma once
#include <iostream>
#include <thread>
#include <functional>
#include "./../../Logger/Logger.h"

class ThreadWrapper {
public:
    ThreadWrapper() : running(false) {}

    ~ThreadWrapper() {
        if (running) {
            stop();
        }
    }

    template<class F, class... Args>
    void start(F&& f, Args&&... args) {
        if (running) {
            throw std::runtime_error("Thread is already running");
        }
        else {
            LOG_MESSAGE(LogLevel::INFO, "Starting thread!");
        }
        thread = std::thread(std::forward<F>(f), std::forward<Args>(args)...);
        running = true;
    }

    void join() {
        if (running) {
            thread.join();
            running = false;
        }
    }

    void stop() {
        if (running) {
            thread.join(); // Wait for the thread to finish
            running = false;
        }
    }

    bool isRunning() const {
        return running;
    }

private:
    std::thread thread;
    bool running;
};

