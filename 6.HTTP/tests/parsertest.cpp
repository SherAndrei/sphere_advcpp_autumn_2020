#include <iostream>
#include <string>
#include "httperr.h"
#include "test_runner.h"
#include "message.h"

void TestRequestStartLine();
void TestResponce();

void TestRequestStartLine() {
    try {
        http::Request r("asdasd");
        ASSERT(false);
    } catch (http::ParsingError& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Invalid request method");
    }

    try {
        http::Request r("GETqwe");
        ASSERT(false);
    } catch (http::ParsingError& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Invalid request method");
    }

    try {
        http::Request r("GET /");
    } catch (http::ParsingError& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Invalid request protocol");
    }

    try {
        http::Request r("GET /asads HTT");
    } catch (http::ParsingError& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Invalid request protocol");
    }

    try {
        http::Request r("GET / HTTP");
    } catch (http::ParsingError& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Invalid request protocol");
    }

    try {
        http::Request r("GET / HTTP/");
    } catch (http::ParsingError& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Invalid request version");
    }

    try {
        http::Request r("GET / HTTP/1.3");
    } catch (http::ParsingError& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Invalid request version");
    }

    try {
        http::Request r("GET / HTTP/1.1asc\r\n");
    } catch (http::ParsingError& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Invalid start line");
    }

    try {
        http::Request r("GET / HTTP/1.1 asd\r\n");
    } catch (http::ParsingError& ex) {
        std::string e(ex.what());
        ASSERT_EQUAL(e, "Invalid start line");
    }

    try {
        http::Request r("GET / HTTP/1.1 \t \r\n");
        ASSERT(r.method() == "GET");
        ASSERT(r.target() == "/");
        ASSERT(r.version() == "1.1");
    } catch (http::ParsingError& ex) {
        ASSERT(false);
    }
    try {
        http::Request r("GET / HTTP/1.1\r\n");
        ASSERT(r.method() == "GET");
        ASSERT(r.target() == "/");
        ASSERT(r.version() == "1.1");
    } catch (http::ParsingError& ex) {
        ASSERT(false);
    }
}

void TestRequestHeaders() {
    try {
        http::Request r("GET / HTTP/1.1\r\nHo:st localHost:8080");
    } catch (http::ParsingError& ex) {
        ASSERT(false);
    }
}

void TestResponce() {

}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestRequestStartLine);
    RUN_TEST(tr, TestRequestHeaders);
    RUN_TEST(tr, TestResponce);
}
