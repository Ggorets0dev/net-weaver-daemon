#include "thread_tasks.hpp"
#include "log.hpp"
#include "build_lists_handler.hpp"

#define RECIEVE_LISTS_TASK_DELAY    1u
#define BUILD_LISTS_TASK_DELAY      20u

// ============ TASKS CONTROL BLOCKS
ThreadTask gBuildListsTask(TaskDelay(BUILD_LISTS_TASK_DELAY));
ThreadTask gRecieveListsTask(TaskDelay(RECIEVE_LISTS_TASK_DELAY));
// ============

// ============ TASKS CORES
TaskCore recieveLists = [](ThreadTask* task) {
    // ============ RECIEVE LISTS TASK-CORE
    // ============
};
// ============

void initAllTasks() {
    auto scheduler = TaskScheduler::getInstance();

    // ============ Set cores to all tasks
    gBuildListsTask.setTaskCore(APPLY_LOOP_TCOR_WRAPPER(gBuildListsCore));
    gRecieveListsTask.setTaskCore(APPLY_LOOP_TCOR_WRAPPER(recieveLists));
    // ============

    // ============ Add all tasks to scheduler
    scheduler->addTask(gBuildListsTask);
    scheduler->addTask(gRecieveListsTask);
    // ============

    LOG_INFO("All tasks were added to scheduler sucessfully");
}
