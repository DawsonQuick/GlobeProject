#pragma once
#ifndef DOUBLEBUFFER_H
#define DOUBLEBUFFER_H

#include <vector>
#include <memory>
#include <mutex>
#include <atomic>

template <typename T>
class DoubleBuffer {
public:
    DoubleBuffer(size_t size)
        : buffer1(std::make_shared<std::vector<T>>(size)),
        buffer2(std::make_shared<std::vector<T>>(size)),
        activeBuffer(buffer1),
        lastCompleteBuffer(buffer1),
        dataReady(false) {}

    // Get a reference to the active buffer
    std::vector<T>& getActiveBuffer() {
        return *std::atomic_load(&lastCompleteBuffer);
    }

    // Swap the buffers
    void swapBuffers() {
        std::lock_guard<std::mutex> lock(bufferMutex);
        auto currentInactive = (activeBuffer == buffer1) ? buffer2 : buffer1;
        std::atomic_store(&activeBuffer, currentInactive);
        std::atomic_store(&lastCompleteBuffer, currentInactive);
        dataReady = true;
    }

    // Get a reference to the inactive buffer for populating
    std::shared_ptr<std::vector<T>> getInactiveBuffer() {
        std::lock_guard<std::mutex> lock(bufferMutex);
        return (activeBuffer == buffer1) ? buffer2 : buffer1;
    }

    // Check if data is ready
    bool isDataReady() const {
        return dataReady;
    }

    // Clear data ready flag
    void clearDataReady() {
        dataReady = false;
    }

private:
    std::shared_ptr<std::vector<T>> buffer1;
    std::shared_ptr<std::vector<T>> buffer2;
    std::shared_ptr<std::vector<T>> activeBuffer;
    std::shared_ptr<std::vector<T>> lastCompleteBuffer;
    std::mutex bufferMutex;  // Used during swap and inactive buffer access
    std::atomic<bool> dataReady;  // Indicates if new data is available
};

#endif // DOUBLEBUFFER_H