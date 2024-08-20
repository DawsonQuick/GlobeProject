#pragma once
#include <vector>
#include <thread>
#include <functional>
#include <queue>
#include <condition_variable>
#include <atomic>

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);
    ~ThreadPool();

    void enqueue(std::function<void()> task);
    void stop(); // Optional: to gracefully stop the pool
    void waitAll(); // Wait for all tasks to complete

    size_t getNumThreads();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stopFlag{ false };
    std::condition_variable tasksDoneCondition; // To signal when all tasks are done
    std::atomic<size_t> pendingTasks{ 0 }; // Track pending tasks
    size_t numberThreadsCount;
};
