#pragma once

#include "core/result.hpp"
#include "transport/http_client.hpp"

#include <string>

namespace pingram::api::users {

class UserRequests {
public:
    UserRequests(const transport::HttpClient& http, std::string base_url);
    core::ApiResult me(const std::string& access_token) const;

private:
    const transport::HttpClient& http_;
    std::string base_url_;
};

} // namespace pingram::api::users
