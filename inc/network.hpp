#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <string>

void sendFile(int sockFd, const std::string& filePath);

void receiveFile(int serverFd, int clientFd, const std::string& outFilePath);

int bindServerSocket(const std::string& ip, int port, int timeoutSec);

int connectToServer(const std::string& ip, int port, int timeoutSec);

#endif // NETWORK_HPP
