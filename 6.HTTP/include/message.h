#ifndef HTTP_MESSAGE_H
#define HTTP_MESSAGE_H
#include <string>
#include <vector>
#include "status_code.h"

namespace http {

struct Header {
    std::string name;
    std::string value;
};

std::string to_string(const Header& header);

class Message {
 public:
    virtual ~Message() = default;

 public:
    virtual std::string str() const = 0;
    virtual void parse(const std::string& message) = 0;

 public:
    std::string version() const;
    std::vector<Header> headers() const;
    std::string body() const;

 protected:
    std::string version_;
    std::vector<Header> headers_;
    std::string body_;
};


class Request : public Message {
 public:
    explicit Request(const std::string& req);

 private:
    friend class HttpConnection;
    Request() = default;

 public:
    std::string str() const override;
    void parse(const std::string& request) override;

 public:
    std::string method() const;
    std::string target() const;

 private:
    std::string method_;
    std::string target_;
};

class Responce : public Message {
 public:
    explicit Responce(const std::string& req);

 private:
    friend class HttpConnection;
    Responce() = default;

 public:
    std::string str() const override;
    void parse(const std::string& responce) override;

 public:
    StatusCode code() const;
    std::string text() const;

 private:
    StatusCode code_{StatusCode::UNKNOWN};
    std::string status_text_;
};

}  // namespace http

#endif  // HTTP_MESSAGE_H

