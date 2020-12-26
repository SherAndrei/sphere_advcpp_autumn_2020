#include <iostream>
#include <algorithm>
#include <string>
#include "httperr.h"
#include "test_runner.h"
#include "message.h"

void TestProtocolParser();
void TestRequestStartLine();
void TestResponceStartLine();
void TestHeaders();
void TestBody();

void TestProtocolParser() {
    try {
        http::Responce r("HTT");
        ASSERT(false);
    } catch (http::ExpectingData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "HTTP/");
    }

    try {
        http::Request r("GET / HTTP");
        ASSERT(false);
    } catch (http::ExpectingData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "HTTP/");
    }

    try {
        http::Responce r("HTTP/");
        ASSERT(false);
    } catch (http::ExpectingData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Request protocol version");
    }
    try {
        http::Responce r("");
        ASSERT(false);
    } catch (http::IncorrectData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Request protocol");
    }

    try {
        http::Responce r("HTTP/1.3");
        ASSERT(false);
    } catch (http::IncorrectData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Request version");
    }

    try {
        http::Request r("GET / HTTP/1.1asc\r\n");
        ASSERT(false);
    } catch (http::IncorrectData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Request version");
    }
}

void TestRequestStartLine() {
    try {
        http::Request r("asdasd");
        ASSERT(false);
    } catch (http::IncorrectData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "asdasd");
    }

    try {
        http::Request r("DELE");
        ASSERT(false);
    } catch (http::ExpectingData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "DELETE");
    }

    try {
        http::Request r("GETqwe");
        ASSERT(false);
    } catch (http::IncorrectData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "GETqwe");
    }

    try {
        http::Request r("GET / HTTP/1.1 asd\r\n");
        ASSERT(false);
    } catch (http::IncorrectData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Start line");
    }

    try {
        http::Request r("GET / HTTP/1.1 \t \r\n\r\n");
        ASSERT(r.method() == "GET");
        ASSERT(r.target() == "/");
        ASSERT(r.version() == "1.1");
    } catch (http::ParsingError& ex) {
        ASSERT(false);
    }
    try {
        http::Request r("GET / HTTP/1.1\r\n\r\n");
        ASSERT(r.method() == "GET");
        ASSERT(r.target() == "/");
        ASSERT(r.version() == "1.1");
    } catch (http::ParsingError& ex) {
        ASSERT(false);
    }
}

void TestResponceStartLine() {
    try {
        http::Responce r("HTTP/1.1 a");
        ASSERT(false);
    } catch (http::IncorrectData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Cannot convert error code");
    }

    try {
        http::Responce r("HTTP/1.1 20");
        ASSERT(false);
    } catch (http::ExpectingData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Status code");
    }

    try {
        http::Responce r("HTTP/1.1 1000");
        ASSERT(false);
    } catch (http::IncorrectData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Status code");
    }

    try {
        http::Responce r("HTTP/1.1 404");
        ASSERT(false);
    } catch (http::ExpectingData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Start line");
    }

    try {
        http::Responce r("HTTP/1.1 404 " + http::to_string(http::StatusCode::NotFound) + "\r\n\r\n");
        ASSERT(r.code() == http::StatusCode::NotFound);
        ASSERT_EQUAL(r.text(), http::to_string(http::StatusCode::NotFound));
    } catch (http::IncorrectData& ex) {
        ASSERT(false);
    }
}

void TestHeaders() {

    try {
        http::Responce r("HTTP/1.1 404\r\n");
        ASSERT(false);
    } catch (http::ExpectingData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Header");
    }
    try {
        http::Request r("GET / HTTP/1.1\r\n\r\n");
        ASSERT_EQUAL(r.headers().size(), 0u);
    } catch (http::ParsingError& ex) {
        ASSERT(false);
    }

    try {
        http::Request r("GET / HTTP/1.1\r\nHo");
    } catch (http::ExpectingData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Header");
    }

    try {
        http::Request r("GET / HTTP/1.1\r\nasdas\r\n\r\n");
        ASSERT(false);
    } catch (http::IncorrectData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Header name");
    }

    try {
        http::Request r("GET / HTTP/1.1\r\n Ho:st 5\r\n\r\n");
        ASSERT(false);
    } catch (http::IncorrectData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Header name");
    }

    try {
        http::Request r("GET / HTTP/1.1\r\n Host:\r\n\r\n");
        ASSERT_EQUAL(r.headers().size(), 1u);
        ASSERT_EQUAL(http::to_string(r.headers()[0]), "Host: \r\n");
    } catch (http::ParsingError& ex) {
        ASSERT(false);
    }

    try {
        http::Request r("GET / HTTP/1.1\r\nHost: \r\n\r\n");
        ASSERT_EQUAL(r.headers().size(), 1u);
        ASSERT_EQUAL(http::to_string(r.headers()[0]), "Host: \r\n");
    } catch (http::ParsingError& ex) {
        ASSERT(false);
    }

    try {
        http::Request r("GET / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
        ASSERT_EQUAL(r.headers().size(), 1u);
        ASSERT_EQUAL(r.headers().begin()->value, "localhost:8080");
    } catch (http::ParsingError& ex) {
        ASSERT(false);
    }

    try {
        http::Request r("GET / HTTP/1.1\r\nHost: localhost:8080\r\n \r\n");
    } catch (http::IncorrectData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Header name");
    }

    try {
        http::Request r("GET / HTTP/1.1\r\nHost: localhost:8080\r\nasdasd\r\n");
    } catch (http::IncorrectData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Header name");
    }

    try {
        http::Request r("GET / HTTP/1.1\r\nHost: localhost:8080\r\nFrom:\r\n");
    } catch (http::ExpectingData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Header");
    }

    try {
        http::Request r("GET / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
        ASSERT_EQUAL(r.headers().size(), 1u);
        ASSERT_EQUAL(r.headers().begin()->value, "localhost:8080");
    } catch (http::ParsingError& ex) {
        ASSERT(false);
    }

    try {
        http::Request r("GET / HTTP/1.1\r\nHost: localhost:8080\r\nFrom: \r\n\r\n");
        ASSERT_EQUAL(r.headers().size(), 2u);
        ASSERT_EQUAL(http::to_string(r.headers()[0]), "Host: localhost:8080\r\n");
        ASSERT_EQUAL(http::to_string(r.headers()[1]), "From: \r\n");
    } catch (http::ParsingError& ex) {
        ASSERT(false);
    }

    try {
        http::Request r("GET / HTTP/1.1\r\n Host: localhost:8080\r\nConnection: Keep-Alive\r\n\r\n");
        ASSERT_EQUAL(r.headers().size(), 2u);
        ASSERT_EQUAL(http::to_string(r.headers()[0]), "Host: localhost:8080\r\n");
        ASSERT_EQUAL(http::to_string(r.headers()[1]), "Connection: Keep-Alive\r\n");
    } catch (http::ParsingError& ex) {
        ASSERT(false);
    }

    try {
        http::Request r("GET / HTTP/1.1\r\n \t \t Host: localhost:8080\r\n  \t  \tConnection: Keep-Alive\r\n\r\n");
        ASSERT_EQUAL(r.headers().size(), 2u);
        ASSERT_EQUAL(http::to_string(r.headers()[0]), "Host: localhost:8080\r\n");
        ASSERT_EQUAL(http::to_string(r.headers()[1]), "Connection: Keep-Alive\r\n");
    } catch (http::ParsingError& ex) {
        ASSERT(false);
    }
    try {
        http::Request r("GET /home/andrew/helloworld.hmtl HTTP/1.0\r\nHost: 127.0.0.1:8080\r\nUser-Agent: ApacheBench/2.3\r\nAccept: */*\r\n\r\n");
        ASSERT_EQUAL(r.headers().size(), 3u);
        ASSERT_EQUAL(http::to_string(r.headers()[0]), "Host: 127.0.0.1:8080\r\n");
        ASSERT_EQUAL(http::to_string(r.headers()[1]), "User-Agent: ApacheBench/2.3\r\n");
        ASSERT_EQUAL(http::to_string(r.headers()[2]), "Accept: */*\r\n");
    } catch (http::ParsingError& ex) {
        ASSERT(false);
    }
}

void TestBody() {
    try {
        http::Request r("GET / HTTP/1.1\r\n\r\n");
        ASSERT_EQUAL(r.body().size(), 0u);
    } catch (http::ParsingError& ex) {
        ASSERT(false);
    }
    try {
        http::Request r("GET / HTTP/1.1\r\n\r\n\r\n");
        ASSERT(false);
    } catch (http::IncorrectData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Body exists, but no content length found");
    }
    try {
        http::Request r("GET / HTTP/1.1\r\n Host: localhost:8080\r\n\r\nasdasd");
        ASSERT(false);
    } catch (http::IncorrectData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Body exists, but no content length found");
    }
    try {
        http::Request r("GET / HTTP/1.1\r\n Host: localhost:8080\r\nContent-Length: \r\n\r\nasdasd");
        ASSERT(false);
    } catch (http::IncorrectData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Content length");
    }
    try {
        http::Request r("GET / HTTP/1.1\r\n Host: localhost:8080\r\nContent-Length: asd\r\n\r\nasdasd");
        ASSERT(false);
    } catch (http::IncorrectData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Content length");
    }
    try {
        http::Request r("GET / HTTP/1.1\r\n Host: localhost:8080\r\nContent-Length: 2\r\n\r\nasdasd");
        ASSERT(false);
    } catch (http::IncorrectData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Size of body");
    }
    try {
        http::Request r("GET / HTTP/1.1\r\n Host: localhost:8080\r\nContent-Length: 6\r\n\r\nas");
        ASSERT(false);
    } catch (http::ExpectingData& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Body");
    }
    try {
        http::Request r("GET / HTTP/1.1\r\nHost: localhost:8080\r\nContent-Length: 13\r\n\r\nTechnosphere!");
        ASSERT_EQUAL(r.headers().size(), 2u);
        ASSERT_EQUAL(http::to_string(r.headers()[0]), "Host: localhost:8080\r\n");
        ASSERT_EQUAL(http::to_string(r.headers()[1]), "Content-Length: 13\r\n");
        ASSERT_EQUAL(r.body(), "Technosphere!");
    } catch (http::ParsingError& ex) {
        ASSERT(false);
    }
}



int main() {
    TestRunner tr;
    RUN_TEST(tr, TestProtocolParser);
    RUN_TEST(tr, TestRequestStartLine);
    RUN_TEST(tr, TestResponceStartLine);
    RUN_TEST(tr, TestHeaders);
    RUN_TEST(tr, TestBody);
}
