#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace pingram::api::messages {

struct SendMessageRequest {
    std::string to_user_id;
    std::string text;
};

inline void to_json(nlohmann::json& j, const SendMessageRequest& r) {
    j = nlohmann::json{{"to_user_id", r.to_user_id}, {"text", r.text}};
}

} // namespace pingram::api::messages
