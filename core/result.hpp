#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace pingram::core {

struct ApiResult {
    int status_code{0};
    nlohmann::json body{};

    [[nodiscard]] bool ok() const {
        return status_code >= 200 && status_code < 300;
    }
};

} // namespace pingram::core
