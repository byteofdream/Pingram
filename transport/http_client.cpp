#include "transport/http_client.hpp"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

#include <nlohmann/json.hpp>

namespace pingram::transport {

QByteArray HttpClient::to_qbyte_array(const std::string& value) {
    return QByteArray(value.c_str(), static_cast<int>(value.size()));
}

core::ApiResult HttpClient::post_json(
    const std::string& url,
    const std::string& json_body,
    const std::map<std::string, std::string>& headers
) const {
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl(QString::fromStdString(url)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    for (const auto& [key, value] : headers) {
        request.setRawHeader(to_qbyte_array(key), to_qbyte_array(value));
    }

    QEventLoop loop;
    QNetworkReply* reply = manager.post(request, to_qbyte_array(json_body));
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    const int status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray body = reply->readAll();
    reply->deleteLater();

    nlohmann::json parsed;
    if (!body.isEmpty()) {
        parsed = nlohmann::json::parse(body.toStdString(), nullptr, false);
    }

    return core::ApiResult{status_code, parsed};
}

core::ApiResult HttpClient::get(
    const std::string& url,
    const std::map<std::string, std::string>& headers
) const {
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl(QString::fromStdString(url)));
    for (const auto& [key, value] : headers) {
        request.setRawHeader(to_qbyte_array(key), to_qbyte_array(value));
    }

    QEventLoop loop;
    QNetworkReply* reply = manager.get(request);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    const int status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray body = reply->readAll();
    reply->deleteLater();

    nlohmann::json parsed;
    if (!body.isEmpty()) {
        parsed = nlohmann::json::parse(body.toStdString(), nullptr, false);
    }

    return core::ApiResult{status_code, parsed};
}

} // namespace pingram::transport
