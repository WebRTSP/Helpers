#pragma once

#include <string>
#include <chrono>


enum {
    TURN_DEFAULT_PORT = 3478,
    TURN_TEMP_PASSWORD_DEFAULT_TTL = 60 * 60, // seconds
};

inline constexpr std::string_view TURN_SCHEME = "turn";

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
    std::string_view scheme,
    const std::string& endpoint);
