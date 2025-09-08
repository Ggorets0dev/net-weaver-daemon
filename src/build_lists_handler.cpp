#include "build_lists_handler.hpp"
#include "log.hpp"

#include "ipc_chain.hpp"
#include "release_notes.pb.h"

#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

struct DataIPC {
    geo_release::ReleaseNotes releaseNotes;
    bool parseStatus;
};

static DataIPC* gpDataIPC;

void handleBuildEndSignal(int sig);

// ============ BUILD LISTS TASK-CORE
TaskCore gBuildListsCore = [](ThreadTask* task) {
    pid_t pid = fork();
    int status;
    DataIPC data;

    gpDataIPC = &data;

    if (pid == 0) {
        // Child process, running RGC...

        LOG_INFO("Fork in BuildListsTask is completed, child process created");

        execl(RGC_INSTALL_PATH, "RuGeolistsCreator", "--child" ,nullptr);

        LOG_ERROR("Failed to run RuGeolistsCreator for building lists");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Adding signal handler for collecting data from FIFO
        signal(SIGUSR1, handleBuildEndSignal);

        // Waiting for RGC to build lists
        waitpid(pid, &status, 0);

        if (errno == EINTR) {
            // Handler was called, wait for PID again...
            waitpid(pid, &status, 0);
        }

        LOG_INFO("Child proccess for building lists is completed");

        if (!data.parseStatus) {
            LOG_ERROR("Failed to parse data from child process or it didnt send anything before exit");
            return;
        }

        // Work with release...
    }
};
// ============

void handleBuildEndSignal(int sig) {
    (void)sig;

    const char* pathFIFO = RGC_RELEASE_NOTES_FIFO_PATH;
    int fd = open(pathFIFO, O_RDONLY);
    bool status;

    if (fd == -1) {
        LOG_ERROR(FILE_OPEN_ERROR_MSG);
        LOG_ERROR("Could not find the FIFO for IPC with child process");
        return;
    }

    gpDataIPC->parseStatus = gpDataIPC->releaseNotes.ParseFromFileDescriptor(fd);

    // Deleting FIFO
    unlink(pathFIFO);
}
