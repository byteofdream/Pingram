#include "api/messages/messages_requests.hpp"

#include <map>

namespace pingram::api::messages {

MessageRequests::MessageRequests(const transport::HttpClient& http, std::string base_url)
    : http_(http), base_url_(std::move(base_url)) {}

core::ApiResult MessageRequests::send_message(const std::string& access_token, const SendMessageRequest& request) const {
    const auto payload = nlohmann::json(request).dump();
    return http_.post_json(
        base_url_ + "/api/v1/messages/send",
        payload,
        std::map<std::string, std::string>{{"Authorization", "Bearer " + access_token}}
    );
}

} // namespace pingram::api::messages
