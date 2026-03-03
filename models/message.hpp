#pragma once

#include <string>

namespace pingram::models {

struct Message {
    std::string id;
    std::string from_user_id;
    std::string to_user_id;
    std::string text;
    std::string created_at;
};

} // namespace pingram::models
