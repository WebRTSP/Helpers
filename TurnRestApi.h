#pragma once

#include <string>
#include <chrono>


std::string TurnTemporaryUsername(
    const std::string& temporaryUsername,
    std::chrono::seconds passwordTTL);

std::string TurnTemporaryPassword(
    const std::string& temporaryUsername,
    const std::string& staticAuthSecret);

std::string GenerateIceServerUrl(
    const std::string& username,
    std::chrono::seconds passwordTTL,
    const std::string& staticAuthSecret,
    const std::string& protocol,
    const std::string& endpoint);
