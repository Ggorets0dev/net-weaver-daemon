#ifndef TASK_SCHEDULER_HPP
#define TASK_SCHEDULER_HPP

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <forward_list>
#include <functional>
#include <thread>

// Tasks delay type before restart
using TaskDelay = std::chrono::minutes;

// Task ID in scheduler's list
using TaskId = uint16_t;

// Function to perform in other thread
using TaskCore = std::function<void()>;

class ThreadTask {
    friend class TaskScheduler;

private:
    TaskCore core_;
    TaskDelay delay_;

    TaskId id_;
    std::atomic<bool> stop_flag_ {false};
    std::condition_variable cv_;
    std::thread thread_;

public:
    ThreadTask(TaskCore core, TaskDelay delay) :
        core_(core), delay_(delay) {};

    ThreadTask(ThreadTask&& other);
};

class TaskScheduler {
private:
    std::mutex mutex_;
    std::forward_list<ThreadTask> tasks_;

    // Singleton
    TaskScheduler() {}

public:
    static TaskScheduler* getInstance();

    bool stopAllTasks();

    TaskId addTask(ThreadTask& task);

    bool startTask(TaskId id);

    bool stopTask(TaskId id);

    ~TaskScheduler();
};

#endif // TASK_SCHEDULER_HPP
