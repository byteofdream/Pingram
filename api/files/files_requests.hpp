#pragma once

#include "core/result.hpp"
#include "transport/http_client.hpp"

#include <string>

namespace pingram::api::files {

class FileRequests {
public:
    FileRequests(const transport::HttpClient& http, std::string base_url);
    core::ApiResult get_file_meta(const std::string& access_token, const std::string& file_id) const;

private:
    const transport::HttpClient& http_;
    std::string base_url_;
};

} // namespace pingram::api::files
