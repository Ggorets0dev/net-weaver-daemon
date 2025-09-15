#ifndef LOG_HPP
#define LOG_HPP

#include <string>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

#include "build_config.hpp"

#define FILE_LOCATE_ERROR_MSG       "Failed to locate file on path: "
#define FILE_OPEN_ERROR_MSG         "Failed to open file on path: "

#define LOG_MARK_ERROR      "❌"
#define LOG_MARK_WARN       "⚠️"
#define LOG_MARK_INFO       "ℹ️"

#ifdef USE_EMOGI_MARKERS
#define LOG_ERROR(msg)          logWithMark(msg, LOG_MARK_ERROR, log4cxx::Level::ERROR_INT)
#define LOG_WARNING(msg)        logWithMark(msg, LOG_MARK_WARN, log4cxx::Level::WARN_INT)
#define LOG_INFO(msg)           logWithMark(msg, LOG_MARK_INFO, log4cxx::Level::INFO_INT)
#else
#define LOG_ERROR(msg)          LOG4CXX_ERROR(gLogger, msg);
#define LOG_WARNING(msg)        LOG4CXX_WARN(gLogger, msg);
#define LOG_INFO(msg)           LOG4CXX_INFO(gLogger, msg);
#endif

using namespace log4cxx;
using namespace log4cxx::helpers;

extern LoggerPtr gLogger;

void initLogging();

void logWithMark(const std::string& msg, const std::string& mark, uint32_t level);

#endif // LOG_HPP
