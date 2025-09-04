#include <algorithm>

#include "task_scheduler.hpp"

static TaskScheduler* gScheduler = nullptr;

// =============================== TASK

void ThreadTask::waitForNotify() {
    auto scheduler = TaskScheduler::getInstance();

    if (this->delay_ == TaskDelay::zero()) {
        // Delay is not required
        return;
    }

    std::unique_lock<std::mutex> lock(*scheduler->getUnionMutex());
    cv_.wait_for(lock, delay_, [this]() {
        return this->stop_flag_.load();
    });
}

void ThreadTask::notifyDeletion() {
    if (schedDelGrp_ == nullptr) {
        // Scheduler is not used, exiting
        return;
    }

    schedDelGrp_->set((1u << id_));
}

TaskCoreWrapper ThreadTask::applyOneShotWrapper(const TaskCore& core) {
    return [&core](ThreadTask* task) {
        core(task);
        task->notifyDeletion();

        return true;
    };
}

TaskCoreWrapper ThreadTask::applyLoopWrapper(const TaskCore& core) {
    return [&core](ThreadTask* task) {
        while (!task->getStopFlag()) {
            core(task);

            // Wait for notify
            task->waitForNotify();
        }

        task->notifyDeletion();

        return true;
    };
}

// ===============================

// =============================== SCHEDULER

void TaskScheduler::stopAllTasks() {
    std::for_each(tasks_.begin(), tasks_.end(), [](auto& task) {
        task->stop_flag_ = true;
        task->cv_.notify_all(); // Waking up running task
    });
}

TaskId TaskScheduler::addTask(ThreadTask& task) {
    TaskId maxId = 0;

    std::for_each(tasks_.begin(), tasks_.end(), [&maxId](const auto& task) {
        maxId = task->id_;
    });

    task.id_ = (maxId + 1);
    task.schedDelGrp_ = &delGrp_;

    tasks_.push_front(&task);

    return task.id_;
}

bool TaskScheduler::startTask(TaskId id) {
    auto iter = std::find_if(tasks_.begin(), tasks_.end(), [&id](const auto& task) {
        return (task->id_ == id);
    });

    bool isFound = (iter != tasks_.end());

    if (isFound) {
        (*iter)->stop_flag_ = false;
        (*iter)->thread_ = std::thread([iter] { (*iter)->core_(*iter); });
    }

    return isFound;
}

bool TaskScheduler::stopTask(TaskId id) {
    auto iter = std::find_if(tasks_.begin(), tasks_.end(), [&id](const auto& task) {
        return (task->id_ == id);
    });

    bool isFound = (iter != tasks_.end());

    if (isFound) {
        (*iter)->stop_flag_ = true;
        (*iter)->cv_.notify_all(); // Waking up running task
    }

    return isFound;
}

void TaskScheduler::removeTask(TaskId id) {
    tasks_.remove_if([&id](const auto& task) {
        return task->id_ == id;
    });
}

TaskScheduler* TaskScheduler::getInstance() {
    if (gScheduler == nullptr) {
        gScheduler = new TaskScheduler();
    }

    return gScheduler;
}

TaskScheduler::~TaskScheduler() {
    if (gScheduler != nullptr) {
        delete gScheduler;
    }
}

std::mutex* TaskScheduler::getUnionMutex() {
    return &mutex_;
}

void TaskScheduler::resetTCB(TaskIterator& iter) {
    (*iter)->stop_flag_ = false;
    // reset other fields
}

void TaskScheduler::resetTCB(TaskId id) {
    auto iter = std::find_if(tasks_.begin(), tasks_.end(), [&id](const auto& task) {
        return (task->id_ == id);
    });

    bool isFound = (iter != tasks_.end());

    if (isFound) {
        resetTCB(iter);
    }
}

void TaskScheduler::controlDelitions() {
    EventFlags::Flags flags;

    while (1) {
        flags = delGrp_.waitAny(std::numeric_limits<EventFlags::Flags>::max());

        for (uint8_t i(0); i < sizeof(flags) * 8; ++i) {
            if (IS_BIT_SET(flags, i)) {
                resetTCB(i);
            }
        }
    }
}

// ===============================

// void TaskScheduler::stopAllTasks() {
//     stop_flag_ = true;
//     cv_.notify_all(); // Waking up all running tasks
// }

// void daily_task() {
//     while (!stop_flag_) {
//         // Выполняем работу
//         std::cout << "Ежедневная задача выполнена" << std::endl;

//         // Ожидаем с возможностью прерывания
//         std::unique_lock<std::mutex> lock(mutex_);
//         cv_.wait_for(lock, std::chrono::hours(24),
//                      [this] { return stop_flag_.load(); });
//     }
// }

