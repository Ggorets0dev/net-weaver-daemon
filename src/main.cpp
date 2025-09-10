// #include <iostream>
#include "thread_tasks.hpp"
#include "cli_args.hpp"
#include "software_info.hpp"
#include "log.hpp"

#define PRINT_DELIMETER "============================"

static void printSoftwareInfo() {
    std::cout << PRINT_DELIMETER << std::endl;
    std::cout << "net-weaver-daemon v" << NV_VERSION_STRING << std::endl;
    std::cout << "Developer: " << NV_DEVELOPER << std::endl;
    std::cout << "License: " << NV_LICENSE << std::endl;
    std::cout << "GitHub: " << NV_REPOSITORY << std::endl;
    std::cout << PRINT_DELIMETER << std::endl;
}

int main(int argc, char** argv) {
    CLI::App app;
    auto scheduler = TaskScheduler::getInstance();

    // Init log handlers
    initLogging();

    // SECTION - Parse CMD args using CLI11 lib
    prepareCmdArgs(app, argc, argv);

    try {
        app.parse(argc, argv);
    } catch (const CLI::CallForHelp &e) {
        std::cout << app.help() << std::endl; // Standard help
        return 0;
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }
    // !SECTION

    // Init tasks with cores in scheduler
    initAllTasks();

    if (gCmdArgs.isClientMode) {
        // OpenWRT tasks ...
        scheduler->startTask(gRecieveListsTask.getTaskId());
        LOG_INFO("All tasks for client mode were started successfully");
    } else if (gCmdArgs.isServerMode) {
        scheduler->startTask(gBuildListsTask.getTaskId());
        LOG_INFO("All tasks for server mode were started successfully");
    } else if (gCmdArgs.isShowAbout) {
        printSoftwareInfo();
        return 0;
    } else {
        // Never going here, CLI11 wont let it happen
    }

    // Block main thread for never return (daemon mode)
    // Main thread is for Scheduler and it's job
    scheduler->controlDelitions();

    return EXIT_SUCCESS;
}
