#include "SpdLog.h"

#ifdef __ANDROID__
#include <spdlog/sinks/android_sink.h>
#else
#include <spdlog/sinks/stdout_sinks.h>
#endif


std::shared_ptr<spdlog::logger> CreateSpdLoggerSt(const std::string& name)
{
#ifdef __ANDROID__
    std::shared_ptr<spdlog::logger> logger = spdlog::android_logger_st(name, name);
    logger->set_pattern("%v");
#else
    std::shared_ptr<spdlog::logger> logger = spdlog::stdout_logger_st(name);

#ifdef SNAPCRAFT_BUILD
    logger->set_pattern("[%l] %v");
#else
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
#endif
#endif

    return logger;
}

std::shared_ptr<spdlog::logger> CreateSpdLoggerMt(const std::string& name)
{
#ifdef __ANDROID__
    std::shared_ptr<spdlog::logger> logger = spdlog::android_logger_mt(name, name);
    logger->set_pattern("%v");
#else
    std::shared_ptr<spdlog::logger> logger = spdlog::stdout_logger_mt(name);

#ifdef SNAPCRAFT_BUILD
    logger->set_pattern("[%l] %v");
#else
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
#endif
#endif

    return logger;
}
