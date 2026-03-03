#include "api/files/files_requests.hpp"

#include <map>

namespace pingram::api::files {

FileRequests::FileRequests(const transport::HttpClient& http, std::string base_url)
    : http_(http), base_url_(std::move(base_url)) {}

core::ApiResult FileRequests::get_file_meta(const std::string& access_token, const std::string& file_id) const {
    return http_.get(
        base_url_ + "/api/v1/files/" + file_id,
        std::map<std::string, std::string>{{"Authorization", "Bearer " + access_token}}
    );
}

} // namespace pingram::api::files
