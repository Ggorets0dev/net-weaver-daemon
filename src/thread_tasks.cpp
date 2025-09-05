#include "thread_tasks.hpp"
#include "log.hpp"
#include "build_lists_handler.hpp"
#include "ipc_chain.hpp"

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define RECIEVE_LISTS_TASK_DELAY    1u
#define BUILD_LISTS_TASK_DELAY      3u

// ============ TASKS CONTROL BLOCKS
ThreadTask gBuildListsTask(TaskDelay(BUILD_LISTS_TASK_DELAY));
ThreadTask gRecieveListsTask(TaskDelay(RECIEVE_LISTS_TASK_DELAY));
// ============

// ============ TASKS CORES
TaskCore buildLists = [](ThreadTask* task) {
    // ============ BUILD LISTS TASK-CORE
    pid_t pid = fork();
    int status;

    if (pid == 0) {
        // Child process, running RGC...

        LOG_INFO("Fork in BuildListsTask is completed, child process created");

        execl(RGC_INSTALL_PATH, "RuGeolistsCreator", nullptr);

        LOG_ERROR("Failed to run RuGeolistsCreator for building lists");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Adding signal handler for collecting data from FIFO
        signal(SIGUSR1, handleBuildEndSignal);

        // Waiting for RGC to build lists
        waitpid(pid, &status, 0);

        bool state = (errno == EINTR);

        LOG_INFO("Child proccess for building lists is completed");
    }
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

    LOG_INFO("All tasks were added to scheduler sucessfully");
}
