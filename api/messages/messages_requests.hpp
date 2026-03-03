#pragma once

#include "api/messages/messages_models.hpp"
#include "core/result.hpp"
#include "transport/http_client.hpp"

#include <string>

namespace pingram::api::messages {

class MessageRequests {
public:
    MessageRequests(const transport::HttpClient& http, std::string base_url);

    core::ApiResult send_message(const std::string& access_token, const SendMessageRequest& request) const;

private:
    const transport::HttpClient& http_;
    std::string base_url_;
};

} // namespace pingram::api::messages
