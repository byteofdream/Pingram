#pragma once

#include <string>

namespace pingram::api::users {

struct UserProfile {
    std::string id;
    std::string username;
    std::string display_name;
};

} // namespace pingram::api::users
