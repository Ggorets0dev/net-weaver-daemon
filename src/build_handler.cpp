#include "build_handler.hpp"
#include "log.hpp"

#include "ipc_chain.hpp"
#include "release_notes.pb.h"
#include "fs_tools.hpp"

#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>

struct DataIPC {
    geo_release::ReleaseNotes releaseNotes;
    bool parseStatus;
    bool sourceActualStatus;
};

static DataIPC* gpDataIPC;

using namespace std;

void handleBuildEndSignal(int sig);

static void printReleaseNotes(const geo_release::ReleaseNotes& release_notes) {
    // Вывод FilesPaths
    cout << "Files Paths:" << endl;
    cout << "  Domain list: " << release_notes.files_paths().domain_list() << endl;
    cout << "  IP list: " << release_notes.files_paths().ip_list() << endl;

    // Вывод времени
    cout << "Time: " << release_notes.time() << endl;

    // Вывод источников
    cout << "Sources:" << endl;
    for (int i = 0; i < release_notes.sources_size(); i++) {
        const geo_release::ReleaseNotes::Source& source = release_notes.sources(i);
        cout << "  Source " << i + 1 << ":" << endl;
        cout << "    Section: " << source.section() << endl;
        cout << "    Type: ";

        // Преобразование enum в строку
        switch (source.type()) {
        case geo_release::ReleaseNotes::DOMAIN:
            cout << "DOMAIN";
            break;
        case geo_release::ReleaseNotes::IP:
            cout << "IP";
            break;
        default:
            cout << "UNKNOWN";
            break;
        }
        cout << endl;
    }
}

// ============ BUILD LISTS TASK-CORE
TaskCore gBuildListsCore = [](ThreadTask* task) {
    pid_t pid = fork();
    int status;
    DataIPC data = {
        .parseStatus = false,
        .sourceActualStatus = false
    };

    gpDataIPC = &data;

    // Adding signal handler for collecting data from FIFO
    signal(SIGUSR1, handleBuildEndSignal);
    signal(SIGUSR2, handleBuildEndSignal);

    if (pid == 0) {
        // Child process, running RGC...

        LOG_INFO("Fork in BuildListsTask is completed, child process created");

        execl(RGC_INSTALL_PATH, "RuGeolistsCreator", "--child", nullptr);

        LOG_ERROR("Failed to run RuGeolistsCreator for building lists");
        return;
    } else if (pid > 0) {
        // Waiting for RGC to build lists
        waitpid(pid, &status, 0);

        if (errno == EINTR) {
            // Handler was called, wait for PID again...
            waitpid(pid, &status, 0);
        }

        LOG_INFO("Child proccess for building lists is completed");

        if (!data.parseStatus && !data.sourceActualStatus) {
            LOG_ERROR("Failed to parse data from child process or it didnt send anything before exit");
            return;
        }

        signal(SIGUSR1, SIG_IGN);
        signal(SIGUSR2, SIG_IGN);

        if (data.sourceActualStatus) {
            LOG_INFO("Check for updates in lists was successfully performed, no need to update");
            return;
        }

        printReleaseNotes(data.releaseNotes);
        // Work with lists
    }
};
// ============

void handleBuildEndSignal(int sig) {
    if (sig == SIGUSR1) {
        // Build is completed, time to recieve data using IPC

        const char* pathFIFO = RGC_RELEASE_NOTES_FIFO_PATH;
        int fd = open(pathFIFO, O_RDONLY);
        bool status;

        if (fd == -1) {
            LOG_ERROR(FILE_OPEN_ERROR_MSG);
            LOG_ERROR("Could not find the FIFO for IPC with child process");
            return;
        }

        gpDataIPC->parseStatus = gpDataIPC->releaseNotes.ParseFromFileDescriptor(fd);

        close(fd);

        if (fs::exists(pathFIFO)) {
            fs::remove(pathFIFO);
        }
    } else { // SIGUSR2
        // Data is actual, no update needed

        gpDataIPC->sourceActualStatus = true;
    }
}
