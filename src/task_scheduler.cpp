#include <algorithm>

#include "task_scheduler.hpp"

static TaskScheduler* gScheduler = nullptr;

// =============================== TASK

ThreadTask::ThreadTask(ThreadTask&& other) {
    this->core_ = std::move(other.core_);
    this->delay_ = other.delay_;
}

// ===============================

// =============================== SCHEDULER

bool TaskScheduler::stopAllTasks() {
    return false;
}

TaskId TaskScheduler::addTask(ThreadTask& task) {
    TaskId maxId;

    std::for_each(tasks_.begin(), tasks_.end(), [&maxId](const auto& task) {
        maxId = task.id_;
    });

    task.id_ = (maxId + 1);

    tasks_.push_front(std::move(task));

    return task.id_;
}

bool TaskScheduler::startTask(TaskId id) {
    for (auto& task : tasks_) {
        if (task.id_ != id) {
            // Not that task is supposed to be started
            continue;
        }

        task.stop_flag_ = false;
        // Взвести cv...?

        task.thread_ = std::thread([&task] { task.core_(); });

        return true;
    }

    return false;
}

bool TaskScheduler::stopTask(TaskId id) {
    for (auto& task : tasks_) {
        if (task.id_ != id) {
            // Not that task is supposed to be stopped
            continue;
        }

        task.stop_flag_ = true;
        task.cv_.notify_all(); // Waking up running task

        return true;
    }

    return false;
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

