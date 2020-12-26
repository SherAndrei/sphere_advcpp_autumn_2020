#include <algorithm>
#include <iostream>
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

    auto pos = sv.find_first_of(" \r");
    auto result = sv.substr(0, pos);
    sv.remove_prefix(pos != sv.npos ? pos : sv.size());
    return result;
}

const std::array<const std::string_view, 10>
METHODS = { "CONNECT", "DELETE", "GET",
            "HEAD", "OPTIONS", "PATCH",
            "POST", "PUT", "TRACE" };

const std::array<const std::string_view, 3>
VERSIONS = { "0.9", "1.0", "1.1" };

const std::array<const std::string_view, 45>
REQUEST_HEADER_NAMES =  { "A-IM", "Accept", "Accept-Charset", "Accept-Control-Request-Headers",
                          "Accept-Control-Request-Method", "Accept-DateTime", "Accept-Encoding",
                          "Accept-Language", "Authorization", "Cache-Control", "Connection",
                          "Content-Encoding", "Content-Length", "Content-MD5", "Content-Type",
                          "Cookie", "DNT", "Date", "Expect", "Forwarded", "From", "Front-End-Https",
                          "HTTP2-Settings", "Host", "If-Match", "If-Modified-Since", "If-None-Match",
                          "If-Unmodified-Since", "IfRange", "Max-Forward", "Origin", "Pragma",
                          "Proxy-Authorization", "Proxy-Connection", "Range", "Referer", "Save-Data",
                          "TE", "Trailer", "Transfer-Encoding", "Upgrade", "Upgrade-Insecure-Requests",
                          "User-Agent", "Via", "Warning" };

const std::array<const std::string_view, 49>
RESPONCE_HEADER_NAMES =  { "Accept-Patch", "Accept-Ranges", "Access-Control-Allow-Headers",
                           "Access-Control-Allow-Methods", "Access-Control-Allow-Origin",
                           "Access-Control-AllowCredentials", "Access-Control-Expose-Headers",
                           "Access-Control-Max-Age", "Age", "Allow", "Alt-Svc", "Cache-Control",
                           "Connection", "Content-Disposition", "Content-Encoding",
                           "Content-Language", "Content-Length", "Content-Locationg",
                           "Content-MD5", "Content-Range", "Content-Security-Policy",
                           "Content-Type", "Date", "Delta-Base", "ETag", "Expires", "IM",
                           "Last-Modified", "Link", "Location", "P3P", "Pragma",
                           "Proxy-Authenticate", "Public-Key-Pins", "Refresh", "Retry-After",
                           "Server", "Set-Cookie", "Status", "Strict-Transport-Security",
                           "Timing-Allow-Origin", "Tk", "Trailer", "Transfer-Encoding", "Upgrade",
                           "Vary", "Via", "WWW-Authenticate", "Warning" };

size_t mismatch(const std::string_view& lhs, const std::string_view& rhs) {
    auto first1 = lhs.begin();
    auto first2 = rhs.begin();
    while (first1 != lhs.end() && first2 != rhs.end() && *first1 == *first2) {
        ++first1, ++first2;
    }
    return first1 - lhs.begin();
}

size_t expecting(const std::string_view& expectation,
               const std::string_view& reality,
               const std::string_view& full_message,
               const std::string& hint = {}) {
    size_t cur = 0u;
    cur = mismatch(expectation, reality);
    if (cur > 0u &&
        cur == reality.length() &&
        cur < expectation.length() &&
        full_message.empty()) {
        throw http::ExpectingData((hint.empty()) ? std::string(expectation) : hint);
    }
    return cur;
}

template<class InputIt>
void expecting_to_find_in_range(InputIt first, InputIt last,
                                const std::string_view& reality,
                                const std::string_view& full_message,
                                const std::string& hint = {}) {
    if (std::find_if(first, last, [&](const std::string_view& sv) {
        return expecting(sv, reality, full_message, hint) == reality.length();
    }) == last) {
        throw http::IncorrectData((hint.empty()) ? std::string(reality) : hint);
    }
}

std::string parse_protocol(std::string_view& mes_sv) {
        std::string_view token = ReadToken(mes_sv);
        size_t cur;
        cur   = expecting("HTTP/", token, mes_sv);
        if (cur == 0) {
            throw http::IncorrectData("Request protocol");
        }
        token.remove_prefix(cur);
        if (token.empty()) {
            throw http::ExpectingData("Request protocol version");
        }
        if (token.size() > 3u) {
            throw http::IncorrectData("Request version");
        }
        expecting_to_find_in_range(VERSIONS.begin(), VERSIONS.end(),
                                token, mes_sv, "Request version");
        return std::string(token);
}

template<class InputIt>
std::vector<http::Header> parse_headers(InputIt begin, InputIt end, std::string_view& mes_sv) {
    size_t cur = 0u;
    std::vector<http::Header> headers;
    std::string_view token;

    while ((cur = mes_sv.find("\r\n")) != mes_sv.npos) {
        if (cur == 0) {
            break;
        }
        token = ReadToken(mes_sv);
        if (token.empty() || (token.find(':') != (token.length() - 1))) {
            throw http::IncorrectData("Header name");
        }
        token.remove_suffix(1);
        if (std::binary_search(begin, end, token) == false && token.find("X-") != 0) {
            throw http::IncorrectData("Header name");
        }
        if (std::find_if(headers.begin(), headers.end(), [token](const http::Header& h) {
                return h.name == token;
            }) != headers.end()) {
            throw http::IncorrectData("Multiple header use");
        }
        LeftStrip(mes_sv);
        cur = mes_sv.find("\r\n");
        headers.emplace_back(http::Header{ std::string(token),
                              std::string(mes_sv.begin(), mes_sv.begin() + cur) });
        mes_sv.remove_prefix(cur + 2);
    }

    if (cur == mes_sv.npos && mes_sv.empty()) {
        throw http::ExpectingData("Header");
    }

    return headers;
}

std::string parse_body(const std::vector<http::Header>& headers, std::string_view& mes_sv) {
    auto it = std::find_if(headers.begin(), headers.end(), [] (const http::Header& h) {
                                return h.name == "Content-Length" || h.name == "Transfer-Encoding";
                                });
    if (it != headers.end()) {
        size_t size;
        try {
            size = std::stoul(it->value);
        } catch (const std::logic_error& ex) {
            throw http::IncorrectData("Content length");
        }
        if (mes_sv.length() < size) {
            throw http::ExpectingData("Body");
        } else if (mes_sv.length() > size) {
            throw http::IncorrectData("Size of body");
        }
        return std::string(mes_sv);
    } else if (!mes_sv.empty()) {
        throw http::IncorrectData("Body exists, but no content length found");
    }

    return {};
}

}  // namespace

namespace http {

std::string to_string(const Header& header) {
    return header.name + ": " + header.value + "\r\n";
}

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

std::string Request::str() const {
    std::string result = method_ + ' '
                       + target_ + ' '
                       + "HTTP/" + version_ + "\r\n";
    for (const Header& header : headers_) {
        result += http::to_string(header);
    }
    result += "\r\n";
    result += body_;
    return result;
}

void Request::parse(const std::string& req) {
    std::string_view req_sv(req);
    size_t cur;
    std::string_view token;

    token = ReadToken(req_sv);
    expecting_to_find_in_range(METHODS.begin(), METHODS.end(), token, req_sv);
    method_ = std::string(token);

    token = ReadToken(req_sv);
    target_ = std::string(token);

    version_ = parse_protocol(req_sv);

    LeftStrip(req_sv);
    token = req_sv.substr(0, 2);
    cur = expecting("\r\n", token, req_sv);
    if (cur == 0) {
        throw IncorrectData("Start line");
    }
    req_sv.remove_prefix(2);
    headers_ = parse_headers(REQUEST_HEADER_NAMES.begin(),
                             REQUEST_HEADER_NAMES.end(), req_sv);
    req_sv.remove_prefix(2);
    body_ = parse_body(headers_, req_sv);
}

Responce::Responce(const std::string& res) {
    parse(res);
}

std::string Request::method() const {
    return method_;
}
std::string Request::target() const {
    return target_;
}

std::string Responce::str() const {
    std::string result = "HTTP/" + version_ + ' '
                       + std::to_string(static_cast<uint16_t>(code_)) + ' '
                       + status_text_ + "\r\n";
    for (const Header& header : headers_) {
        result += http::to_string(header);
    }
    result += "\r\n";
    result += body_;
    return result;
}
void Responce::parse(const std::string& responce) {
    std::string_view res_sv(responce);
    std::string_view token;
    size_t cur;

    version_ = parse_protocol(res_sv);

    token = ReadToken(res_sv);
    try {
        code_ = static_cast<StatusCode>(std::stoul(std::string(token)));
    } catch (const std::logic_error& ex) {
        throw IncorrectData("Cannot convert error code");
    }
    // Так как кодов ошибки > 6** нет
    if (static_cast<size_t>(code_) < 60u && res_sv.empty()) {
        throw ExpectingData("Status code");
    }
    if (code_ > StatusCode::NetworkConnectTimeoutError) {
        throw IncorrectData("Status code");
    }

    LeftStrip(res_sv);
    cur = res_sv.find("\r\n");
    if (cur == res_sv.npos) {
        throw ExpectingData("Start line");
    }
    status_text_ = std::string(res_sv.begin(), res_sv.begin() + cur);
    res_sv.remove_prefix(cur + 2);
    headers_ = parse_headers(RESPONCE_HEADER_NAMES.begin(),
                             RESPONCE_HEADER_NAMES.end(), res_sv);
    res_sv.remove_prefix(2);
    body_    = parse_body(headers_, res_sv);
}

StatusCode Responce::code() const {
    return code_;
}
std::string Responce::text() const {
    return status_text_;
}

}  // namespace http
