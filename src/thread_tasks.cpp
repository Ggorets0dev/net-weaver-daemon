#include "thread_tasks.hpp"
#include "log.hpp"
#include "build_config.hpp"

#include "build_handler.hpp"
#include "transmit_handler.hpp"
#include "receive_handler.hpp"

// ============ TASKS CONTROL BLOCKS
ThreadTask gBuildListsTask(TaskDelay(BUILD_LISTS_TASK_DELAY));
ThreadTask gReceiveListsTask(TaskDelay(RECEIVE_LISTS_TASK_DELAY));
ThreadTask gTransmitListsTask(TaskDelay(TRANSMIT_LISTS_TASK_DELAY));
// ============

void initAllTasks() {
    auto scheduler = TaskScheduler::getInstance();

    // ============ Set cores to all tasks
    gBuildListsTask.setTaskCore(APPLY_LOOP_TCOR_WRAPPER(gBuildListsCore));
    gTransmitListsTask.setTaskCore(APPLY_LOOP_TCOR_WRAPPER(gTransmitListsCore));
    gReceiveListsTask.setTaskCore(APPLY_LOOP_TCOR_WRAPPER(gReceiveListsCore));
    // ============

    // ============ Add all tasks to scheduler
    scheduler->addTask(gBuildListsTask);
    scheduler->addTask(gTransmitListsTask);
    scheduler->addTask(gReceiveListsTask);
    // ============

    LOG_INFO("All tasks were added to scheduler sucessfully");
}
