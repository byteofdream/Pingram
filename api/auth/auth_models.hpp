#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace pingram::api::auth {

struct LoginRequest {
    std::string phone;
    std::string password;
};

struct RegisterRequest {
    std::string phone;
    std::string password;
    std::string username;
};

struct LoginResponse {
    std::string access_token;
    std::string token_type;
};

inline void to_json(nlohmann::json& j, const LoginRequest& r) {
    j = nlohmann::json{{"phone", r.phone}, {"password", r.password}};
}

inline void to_json(nlohmann::json& j, const RegisterRequest& r) {
    j = nlohmann::json{{"phone", r.phone}, {"password", r.password}, {"username", r.username}};
}

} // namespace pingram::api::auth
