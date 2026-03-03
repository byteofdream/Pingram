#pragma once

#include "core/result.hpp"

#include <QByteArray>

#include <map>
#include <string>

namespace pingram::transport {

class HttpClient {
public:
    core::ApiResult post_json(
        const std::string& url,
        const std::string& json_body,
        const std::map<std::string, std::string>& headers = {}
    ) const;

    core::ApiResult get(
        const std::string& url,
        const std::map<std::string, std::string>& headers = {}
    ) const;

private:
    static QByteArray to_qbyte_array(const std::string& value);
};

} // namespace pingram::transport
