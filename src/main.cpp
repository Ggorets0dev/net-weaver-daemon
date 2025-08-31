// #include <iostream>
#include "thread_tasks.hpp"

int main()
{
    initAllTasks();

    auto scheduler = TaskScheduler::getInstance();

    scheduler->startTask(gBuildListsTask.getTaskId());

    std::this_thread::sleep_for(std::chrono::seconds(10));

    scheduler->stopTask(gBuildListsTask.getTaskId());

    while(1);

    return 0;
}
