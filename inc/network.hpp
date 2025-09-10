#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <string>

void sendFile(const std::string& ip, int port, const std::string& filePath);

void receiveFile(const std::string& ip, int port, const std::string& outFilePath);

#endif // NETWORK_HPP
