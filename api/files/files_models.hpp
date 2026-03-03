#pragma once

#include <string>

namespace pingram::api::files {

struct UploadFileResponse {
    std::string file_id;
    std::string url;
};

} // namespace pingram::api::files
