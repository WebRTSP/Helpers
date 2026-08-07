#pragma once

#include <spdlog/spdlog.h>


std::shared_ptr<spdlog::logger> CreateSpdLoggerSt(const std::string& name);
std::shared_ptr<spdlog::logger> CreateSpdLoggerMt(const std::string& name);
