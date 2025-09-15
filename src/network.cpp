#include <iostream>
#include <fstream>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>

#include "network.hpp"
#include "log.hpp"

#define BUFFER_SIZE     4096u

static bool connectWithTimeout(int sockfd, const sockaddr* addr, socklen_t addrlen, int timeoutSec) {
    // 1. Setting non-block mode
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    // 2. Trying to connect
    int result = connect(sockfd, addr, addrlen);
    if (result == 0) {
        // Connection is completed
        fcntl(sockfd, F_SETFL, flags); // Return to block mode
        return true;
    } else if (errno != EINPROGRESS) {
        // Connection error
        return false;
    }

    // 3. Waiting for socket to get ready with select()
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(sockfd, &writefds);

    timeval tv{};
    tv.tv_sec = timeoutSec;
    tv.tv_usec = 0;

    result = select(sockfd + 1, nullptr, &writefds, nullptr, &tv);
    if (result <= 0) {
        // Timeout or error
        return false;
    }

    // 4. Checking if connection exists
    int so_error;
    socklen_t len = sizeof(so_error);
    getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &so_error, &len);

    // Return back to block mode
    fcntl(sockfd, F_SETFL, flags);

    return so_error == 0;
}

void receiveFile(int serverFd, int clientFd, const std::string& outFilePath) {
    struct sockaddr_in clientAddr{};
    socklen_t clientLen = sizeof(clientAddr);
    char buffer[BUFFER_SIZE];
    ssize_t bytesReceived;

    // LOG_INFO("Waiting for connection on server side...");
    // clientFd = accept(sockFd, (struct sockaddr*)&clientAddr, &clientLen);

    // if (clientFd == -1) {
    //     throw std::runtime_error("Failed to accept ")
    //     close(serverFd);
    //     return;
    // }

    std::ofstream output(outFilePath, std::ios::binary);

    if (!output.is_open()) {
        // throw
        close(clientFd);
        close(serverFd);
        return;
    }

    while ((bytesReceived = recv(clientFd, buffer, BUFFER_SIZE, 0)) > 0) {
        output.write(buffer, bytesReceived);
    }

    LOG_INFO("Successfully collected and saved file from client: " + outFilePath);

    output.close();
}

void sendFile(int sockFd, const std::string& filePath) {
    char buffer[BUFFER_SIZE];

    std::ifstream input(filePath, std::ios::binary);
    if (!input.is_open()) {
        LOG_ERROR("Failed to open file: " + filePath);
        close(sockFd);
        return;
    }

    while (!input.eof()) {
        input.read(buffer, BUFFER_SIZE);
        std::streamsize bytesRead = input.gcount();
        if (send(sockFd, buffer, bytesRead, 0) == -1) {
            LOG_ERROR("Send failed");
            break;
        }
    }

    LOG_INFO("File " + filePath + " sent successfully");

    input.close();
    close(sockFd);
}

int bindServerSocket(const std::string& ip, int port, int timeoutSec) {
    int serverFd, clientFd;
    struct sockaddr_in serverAddr{};

    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1) {
        throw std::runtime_error("Failed to create server socket");
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

    if (bind(serverFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        close(serverFd);
        std::runtime_error("Failed to bind server socket");
    }

    if (listen(serverFd, 1) == -1) {
        close(serverFd);
        throw std::runtime_error("Failed to start listening on server socket");
    }

    return serverFd;
}

int connectToServer(const std::string& ip, int port, int timeoutSec) {
    bool status;
    int sockFd;
    struct sockaddr_in serverAddr{};

    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd == -1) {
        throw std::runtime_error("Failed to create socket");
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

    status = connectWithTimeout(sockFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr), timeoutSec);

    if (status) {
        return sockFd;
    } else {
        throw std::runtime_error("Failed to connect to client");
    }
}

