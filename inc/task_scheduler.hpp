#ifndef TASK_SCHEDULER_HPP
#define TASK_SCHEDULER_HPP

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <forward_list>
#include <functional>
#include <thread>

#include "event_flag_grp.hpp"
#include "build_config.hpp"

#ifdef USE_DELAY_UNITS_SEC
// Tasks delay type before restart
using TaskDelay = std::chrono::seconds;
#elif USE_DELAY_UNITS_MIN
using TaskDelay = std::chrono::minutes;
#elif USE_DELAY_UNITS_HOURS
using TaskDelay = std::chrono::hours;
#endif

#define APPLY_ONESHOT_TCOR_WRAPPER(core)    ThreadTask::applyOneShotWrapper(core)
#define APPLY_LOOP_TCOR_WRAPPER(core)       ThreadTask::applyLoopWrapper(core)

// forward-declaration
class ThreadTask;

// Task ID in scheduler's list
using TaskId = uint16_t;

// Function to perform in other thread
using TaskCore = std::function<void(ThreadTask*)>;

// Wrapper for task core with different types of behaviour
using TaskCoreWrapper = std::function<bool(ThreadTask*)>;

class ThreadTask {
    friend class TaskScheduler;

private:
    TaskCoreWrapper core_;
    TaskDelay delay_;
    EventFlags::EventFlagGroup* schedDelGrp_ {nullptr};

    TaskId id_;
    std::atomic<bool> stop_flag_ {false};
    std::condition_variable cv_;
    std::thread thread_;

public:
    ThreadTask(TaskDelay delay) :
        delay_(delay) {};

    ThreadTask(ThreadTask&& other);

    void waitForNotify();

    void notifyDeletion();

    void setTaskCore(TaskCoreWrapper core) { core_ = core; };

    bool getStopFlag() { return stop_flag_.load(); }

    TaskId getTaskId() { return id_; }

    static TaskCoreWrapper applyOneShotWrapper(const TaskCore& core);
    static TaskCoreWrapper applyLoopWrapper(const TaskCore& core);
};

class TaskScheduler {
private:
    using TasksContainer = std::forward_list<ThreadTask*>;
    using TaskIterator = TasksContainer::iterator;

    std::mutex mutex_;
    TasksContainer tasks_;
    EventFlags::EventFlagGroup delGrp_;

    // Singleton
    TaskScheduler() {}

    void resetTCB(TaskIterator& iter);
    void resetTCB(TaskId id);

public:
    static TaskScheduler* getInstance();

    void stopAllTasks();

    TaskId addTask(ThreadTask& task);

    bool startTask(TaskId id);

    bool stopTask(TaskId id);

    void removeTask(TaskId id);

    std::mutex* getUnionMutex();

    void controlDelitions();

    ~TaskScheduler();
};

#endif // TASK_SCHEDULER_HPP
