#include <iostream>

#include "log.hpp"
#include "fs_tools.hpp"

LoggerPtr gLogger(Logger::getLogger("NV"));

const fs::path gkLogConfigPath = fs::path(std::getenv("HOME")) / ".config" / "net-weaver" / "log4cxx.properties";

void initLogging() {
    try {
        PropertyConfigurator::configure(gkLogConfigPath.string());
    } catch (const Exception& e) {
        std::cerr << "Failed to load log4cxx settings: " << e.what() << std::endl;
        exit(1);
    }
}

void logWithMark(const std::string& msg, const std::string& mark, uint32_t level) {
    std::string markMsg;
    markMsg.reserve(msg.length() + mark.length() + 1);

    markMsg = mark + "  " + msg;

    switch (level) {
    case log4cxx::Level::ERROR_INT:
        LOG4CXX_ERROR(gLogger, markMsg.c_str());
        break;
    case log4cxx::Level::WARN_INT:
        LOG4CXX_WARN(gLogger, markMsg.c_str());
        break;
    case log4cxx::Level::INFO_INT:
        LOG4CXX_INFO(gLogger, markMsg.c_str());
        break;
    default:
        // Level not supported
        break;
    }
}
