#include <algorithm>
#include <array>
#include <string_view>
#include "httperr.h"
#include "message.h"

namespace {

bool is_space(char c) {
    return (c == ' ' || c == '\t');
}

void LeftStrip(std::string_view& sv) {
    while (!sv.empty() && is_space(sv[0])) {
        sv.remove_prefix(1);
    }
}

std::string_view ReadToken(std::string_view& sv) {
    LeftStrip(sv);

    auto pos = sv.find(' ');
    auto result = sv.substr(0, pos);
    sv.remove_prefix(pos != sv.npos ? pos : sv.size());
    return result;
}

const std::array<const std::string_view, 10>
METHODS = { "OPTIONS", "GET", "HEAD", "POST",
            "PUT", "PATCH", "DELETE", "TRACE", "CONNECT" };

const std::array<const std::string_view, 3>
VERSIONS = { "0.9", "1.0", "1.1" };

}  // namespace

std::vector<http::Header> parse_headers(std::string_view& mes_sv) {
    size_t cur = 0u;
    std::vector<http::Header> headers;
    std::string_view token;

    while (mes_sv.find("\r\n") != mes_sv.npos) {
        token = ReadToken(mes_sv);
        if (token.find(':') != token.back()) {
            throw http::ParsingError("Invalid header name");
        }
        token.remove_suffix(1);
        LeftStrip(mes_sv);
        cur = mes_sv.find("\r\n");
        headers.emplace_back(std::string(token),
                              std::string(mes_sv.begin(), mes_sv.begin() + cur));
        mes_sv.remove_prefix(cur + 2);
    }


}


namespace http {

std::string Message::version() const {
    return version_;
}
std::vector<Header> Message::headers() const {
    return headers_;
}
std::string Message::body() const {
    return body_;
}

Request::Request(const std::string& req) {
    parse(req);
}

std::string Request::to_string() const {
    std::string result = method_ + ' '
                       + target_ + ' '
                       + "HTTP/" + version_ + "\r\n";
    for (const Header& header : headers_) {
        result += header.name + ": " + header.value + "\r\n";
    }
    result += "\r\n";
    result += body_;
    return result;
}

void Request::parse(const std::string& req) {
    std::string_view req_sv(req);

    //  START LINE

    std::string_view token = ReadToken(req_sv);
    if (std::find(METHODS.begin(), METHODS.end(), token) == METHODS.end()) {
        throw ParsingError("Invalid request method");
    }
    method_ = std::string(token);

    token = ReadToken(req_sv);
    target_ = std::string(token);

    token = ReadToken(req_sv);
    size_t cur = token.find("HTTP/");
    if (cur != 0) {
      throw ParsingError("Invalid request protocol");
    }
    token.remove_prefix(cur + 5);
    if (std::find(VERSIONS.begin(), VERSIONS.end(), token.substr(0, 3)) == VERSIONS.end()) {
      throw ParsingError("Invalid request version");
    }
    version_ = std::string(token.substr(0, 3));
    token.remove_prefix(3);

    cur = token.find("\r\n");
    if (cur != 0) {
        LeftStrip(req_sv);
        if (req_sv.find("\r\n") != 0) {
            throw ParsingError("Invalid start line");
        }
        req_sv.remove_prefix(2);
    }

    LeftStrip(req_sv);

    headers_ = parse_headers(req_sv);

    if (!req_sv.empty()) {
        auto it = std::find_if(headers_.begin(), headers_.end(), [] (const http::Header& h) {
                                    return h.name == "Content-Length";
                                    });
        if (it != headers_.end()) {
            size_t size = std::stoul(it->value);
            if (req_sv.length() != size) {
                throw http::ParsingError("Invalid length of body");
            }
            body_ = std::string(req_sv);
        } else {
            throw ParsingError("Invalid headers: Body exists, no contnent length found");
        }
    }
}

std::string Request::method() const {
    return method_;
}
std::string Request::target() const {
    return target_;
}

std::string Responce::to_string() const {
    return {};
}
void Responce::parse(const std::string& responce) {
    (void) responce;
}

}  // namespace http
