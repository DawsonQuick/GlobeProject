#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t numThreads) : numberThreadsCount(numThreads) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    condition.wait(lock, [this] { return stopFlag || !tasks.empty(); });
                    if (stopFlag && tasks.empty()) {
                        return;
                    }
                    task = std::move(tasks.front());
                    tasks.pop();
                    pendingTasks++;
                }
                task();
                pendingTasks--;
                tasksDoneCondition.notify_one();
            }
            });
    }
}

ThreadPool::~ThreadPool() {
    stop();
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        if (stopFlag) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        tasks.emplace(std::move(task));
    }
    condition.notify_one();
}

void ThreadPool::stop() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stopFlag = true;
    }
    condition.notify_all();
    for (std::thread& worker : workers) {
        worker.join();
    }
}

void ThreadPool::waitAll() {
    std::unique_lock<std::mutex> lock(queueMutex);
    tasksDoneCondition.wait(lock, [this] { return pendingTasks == 0 && tasks.empty(); });
}

size_t ThreadPool::getNumThreads() {
    return numberThreadsCount;
}
