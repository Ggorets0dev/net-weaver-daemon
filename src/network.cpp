#include <iostream>
#include <fstream>
#include <arpa/inet.h>
#include <unistd.h>

#include "network.hpp"
#include "log.hpp"

#define BUFFER_SIZE     4096u

void receiveFile(const std::string& ip, int port, const std::string& outFilePath) {
    int serverFd, clientFd;
    struct sockaddr_in serverAddr{}, clientAddr{};
    socklen_t clientLen = sizeof(clientAddr);
    char buffer[BUFFER_SIZE];
        ssize_t bytesReceived;

    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1) {
        LOG_ERROR("Socket creation failed");
        return;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

    if (bind(serverFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        LOG_ERROR("Bind failed");
        close(serverFd);
        return;
    }

    if (listen(serverFd, 1) == -1) {
        LOG_ERROR("Listen failed");
        close(serverFd);
        return;
    }

    LOG_INFO("Waiting for connection...");
    clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &clientLen);

    if (clientFd == -1) {
        LOG_ERROR("Accept failed");
        close(serverFd);
        return;
    }

    std::ofstream output(outFilePath, std::ios::binary);

    if (!output.is_open()) {
        LOG_ERROR("Failed to open output file");
        close(clientFd);
        close(serverFd);
        return;
    }

    while ((bytesReceived = recv(clientFd, buffer, BUFFER_SIZE, 0)) > 0) {
        output.write(buffer, bytesReceived);
    }

    LOG_INFO("File received and saved");

    output.close();
    close(clientFd);
    close(serverFd);
}

void sendFile(const std::string& ip, int port, const std::string& filePath) {
    int sockFd;
    struct sockaddr_in serverAddr{};
    char buffer[BUFFER_SIZE];

    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd == -1) {
        LOG_ERROR("Socket creation failed");
        return;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

    if (connect(sockFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        LOG_ERROR("Connection failed");
        close(sockFd);
        return;
    }

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
