#pragma once

#include "api/auth/auth_models.hpp"
#include "core/result.hpp"
#include "transport/http_client.hpp"

#include <string>

namespace pingram::api::auth {

class AuthRequests {
public:
    AuthRequests(const transport::HttpClient& http, std::string base_url);

    core::ApiResult login(const LoginRequest& request) const;
    core::ApiResult register_account(const RegisterRequest& request) const;

private:
    const transport::HttpClient& http_;
    std::string base_url_;
};

} // namespace pingram::api::auth
