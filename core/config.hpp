#pragma once

#include <string>

namespace pingram::core {

struct Config {
    std::string base_url{"http://127.0.0.1:8000"};
    std::string access_token{};
};

} // namespace pingram::core
