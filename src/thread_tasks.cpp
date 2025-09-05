#include "thread_tasks.hpp"

#define RECIEVE_LISTS_TASK_DELAY    1u
#define BUILD_LISTS_TASK_DELAY      3u

// ============ TASKS CONTROL BLOCKS
ThreadTask gBuildListsTask(TaskDelay(BUILD_LISTS_TASK_DELAY));
ThreadTask gRecieveListsTask(TaskDelay(RECIEVE_LISTS_TASK_DELAY));
// ============

// ============ TASKS CORES
TaskCore buildLists = [](ThreadTask* task) {
    // ============ BUILD LISTS TASK-CORE
    // ============
};

TaskCore recieveLists = [](ThreadTask* task) {
    // ============ RECIEVE LISTS TASK-CORE
    // ============
};
// ============

void initAllTasks() {
    auto scheduler = TaskScheduler::getInstance();

    // ============ Set cores to all tasks
    gBuildListsTask.setTaskCore(APPLY_LOOP_TCOR_WRAPPER(buildLists));
    gRecieveListsTask.setTaskCore(APPLY_LOOP_TCOR_WRAPPER(recieveLists));
    // ============

    // ============ Add all tasks to scheduler
    scheduler->addTask(gBuildListsTask);
    scheduler->addTask(gRecieveListsTask);
    // ============
}
