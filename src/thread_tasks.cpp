#include "thread_tasks.hpp"
#include "log.hpp"
#include "build_lists_handler.hpp"
#include "build_config.hpp"

// ============ TASKS CONTROL BLOCKS
ThreadTask gBuildListsTask(TaskDelay(BUILD_LISTS_TASK_DELAY));
ThreadTask gReceiveListsTask(TaskDelay(RECEIVE_LISTS_TASK_DELAY));
// ============

// ============ TASKS CORES
TaskCore receiveLists = [](ThreadTask* task) {
    // ============ RECIEVE LISTS TASK-CORE
    // ============
};
// ============

void initAllTasks() {
    auto scheduler = TaskScheduler::getInstance();

    // ============ Set cores to all tasks
    gBuildListsTask.setTaskCore(APPLY_LOOP_TCOR_WRAPPER(gBuildListsCore));
    gReceiveListsTask.setTaskCore(APPLY_LOOP_TCOR_WRAPPER(receiveLists));
    // ============

    // ============ Add all tasks to scheduler
    scheduler->addTask(gBuildListsTask);
    scheduler->addTask(gReceiveListsTask);
    // ============

    LOG_INFO("All tasks were added to scheduler sucessfully");
}
