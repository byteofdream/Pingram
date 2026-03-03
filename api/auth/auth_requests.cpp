#include "api/auth/auth_requests.hpp"

namespace pingram::api::auth {

AuthRequests::AuthRequests(const transport::HttpClient& http, std::string base_url)
    : http_(http), base_url_(std::move(base_url)) {}

core::ApiResult AuthRequests::login(const LoginRequest& request) const {
    const auto payload = nlohmann::json(request).dump();
    return http_.post_json(base_url_ + "/api/v1/auth/login", payload);
}

core::ApiResult AuthRequests::register_account(const RegisterRequest& request) const {
    const auto payload = nlohmann::json(request).dump();
    return http_.post_json(base_url_ + "/api/v1/auth/register", payload);
}

} // namespace pingram::api::auth
