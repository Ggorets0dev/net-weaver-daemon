#include "build_lists_handler.hpp"
#include "log.hpp"
#include "ipc_chain.hpp"

#include <fcntl.h>
#include <unistd.h>

void handleBuildEndSignal(int sig) {
    const char* pathFIFO = RGC_RELEASE_NOTES_FIFO_PATH;
    int fd = open(pathFIFO, O_RDONLY);
    char buffer[256];
    int bytesCount;

    if (fd == -1) {
        LOG_ERROR(FILE_OPEN_ERROR_MSG);
        LOG_ERROR("Could not find the FIFO for IPC with child process");
        return;
    }

    bytesCount = read(fd, buffer, sizeof(buffer) - 1);

    close(fd);

    if (bytesCount > 0) {
        buffer[bytesCount] = '\0';
        LOG_INFO("Data from child process was collected successfully");
    } else {
        LOG_ERROR("Could not find the data that the child process was supposed to send");
    }

    // Deleting FIFO
    unlink(pathFIFO);
}
