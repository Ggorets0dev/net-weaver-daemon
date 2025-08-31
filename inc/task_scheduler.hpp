#ifndef TASK_SCHEDULER_HPP
#define TASK_SCHEDULER_HPP

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <forward_list>
#include <functional>
#include <thread>

// For debug. In real work there is no task with seconds delay
#define TASK_DELAY_TYPE_SEC

#ifdef TASK_DELAY_TYPE_SEC
// Tasks delay type before restart
using TaskDelay = std::chrono::seconds;
#else
using TaskDelay = std::chrono::minutes;
#endif

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
    ThreadTask(TaskDelay delay) :
        delay_(delay) {};

    ThreadTask(ThreadTask&& other);

    void waitForNotify();

    void setTaskCore(TaskCore&& core) { core_ = std::move(core); };

    bool getStopFlag() { return stop_flag_.load(); }

    TaskId getTaskId() { return id_; }
};

class TaskScheduler {
private:
    std::mutex mutex_;
    std::forward_list<ThreadTask*> tasks_;

    // Singleton
    TaskScheduler() {}

public:
    static TaskScheduler* getInstance();

    void stopAllTasks();

    TaskId addTask(ThreadTask& task);

    bool startTask(TaskId id);

    bool stopTask(TaskId id);

    void removeTask(TaskId id);

    std::mutex* getUnionMutex();

    ~TaskScheduler();
};

#endif // TASK_SCHEDULER_HPP
