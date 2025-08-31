#include "thread_tasks.hpp"
#include <iostream>

#define BUILD_LISTS_TASK_DELAY      2u

// ============ TASKS CONTROL BLOCKS
ThreadTask gBuildListsTask(TaskDelay(BUILD_LISTS_TASK_DELAY));
// ============

// ============ TASKS CORES
TaskCore buildLists = []() {
    while (!gBuildListsTask.getStopFlag()) {
        // >>>
        // TASK
        std::cout << "OK" << std::endl;
        // <<<

        // Wait for notify
        gBuildListsTask.waitForNotify();
    }

    std::cout << "END" << std::endl;
};
// ============

void initAllTasks() {
    auto scheduler = TaskScheduler::getInstance();

    // ============ Set cores to all tasks
    gBuildListsTask.setTaskCore(std::move(buildLists));
    // ============

    // ============ Add all tasks to scheduler
    scheduler->addTask(gBuildListsTask);
    // ============
}
