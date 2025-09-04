#ifndef THREAD_TASKS_HPP
#define THREAD_TASKS_HPP

#include "task_scheduler.hpp"

extern ThreadTask gBuildListsTask;
extern ThreadTask gRecieveListsTask;

void initAllTasks();

#endif // THREAD_TASKS_HPP
