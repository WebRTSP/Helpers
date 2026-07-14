#pragma once

#include <string>
#include <deque>
#include <optional>


std::deque<std::string> ConfigDirs();
std::optional<std::string> DataDir();
std::string FullPath(const std::string& configDir, const std::string& path);
