#include "build_lists_handler.hpp"
#include "log.hpp"

#include <fcntl.h>
#include <unistd.h>

void handleBuildEndSignal(int sig) {
#warning "Correct name of FIFO is required"
    const char* pathFIFO = "data.fifo";
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
