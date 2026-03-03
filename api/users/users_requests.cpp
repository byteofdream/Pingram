#include "api/users/users_requests.hpp"

#include <map>

namespace pingram::api::users {

UserRequests::UserRequests(const transport::HttpClient& http, std::string base_url)
    : http_(http), base_url_(std::move(base_url)) {}

core::ApiResult UserRequests::me(const std::string& access_token) const {
    return http_.get(
        base_url_ + "/api/v1/users/me",
        std::map<std::string, std::string>{{"Authorization", "Bearer " + access_token}}
    );
}

} // namespace pingram::api::users
