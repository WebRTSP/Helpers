#include "TurnRestApi.h"

#include <glib.h>

#include <cassert>
#include <vector>

std::string TurnTemporaryUsername(
    const std::string& username,
    std::chrono::seconds passwordTTL)
{
    const time_t now = time(nullptr);

    if(username.empty())
        return std::to_string(now + passwordTTL.count());
    else
        return std::to_string(now + passwordTTL.count()) + ":" + username;
}

std::string TurnTemporaryPassword(
    const std::string& temporaryUsername,
    const std::string& staticAuthSecret)
{
    const GChecksumType type = G_CHECKSUM_SHA1;
    const gssize digestSize = g_checksum_type_get_length(type);
    if(-1 == digestSize)
        return std::string();

    GHmac* hmac =
        g_hmac_new(
            type,
            reinterpret_cast<const guchar*>(staticAuthSecret.c_str()),
            staticAuthSecret.size());
    if(hmac) {
        g_hmac_update(
            hmac,
            reinterpret_cast<const guchar*>(temporaryUsername.c_str()),
            temporaryUsername.size());

        std::vector<guint8> digest(digestSize);
        gsize digestLen = digest.size();
        g_hmac_get_digest(hmac, digest.data(), &digestLen);
        assert(digestLen == static_cast<const gsize>(digestSize));

        g_hmac_unref(hmac);

        gchar* base64 = g_base64_encode(digest.data(), digest.size());
        std::string result = base64;
        g_free(base64);

        return result;
    }

    return std::string();
}

std::string GenerateIceServerUrl(
    const std::string& username,
    std::chrono::seconds passwordTTL,
    const std::string& staticAuthSecret,
    const std::string& protocol,
    const std::string& endpoint)
{
    const std::string temporaryUsername =
        TurnTemporaryUsername(username, passwordTTL);

    const std::string password =
        TurnTemporaryPassword(temporaryUsername, staticAuthSecret);

    gchar* escapedUserName = g_uri_escape_string(temporaryUsername.c_str(), nullptr, false);
    gchar* escapedPassword = g_uri_escape_string(password.c_str(), nullptr, false);

    std::string result = protocol + escapedUserName + ":" + escapedPassword + "@" + endpoint;

    g_free(escapedUserName);
    g_free(escapedPassword);

    return result;
}
