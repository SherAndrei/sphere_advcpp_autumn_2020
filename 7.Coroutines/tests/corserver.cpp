#include <iostream>
#include "globallogger.h"
#include "address.h"
#include "tcperr.h"
#include "corservice.h"

class TestListener : public http::cor::ICoroutineListener {
    http::Responce OnRequest(http::Request&& req) override {
        (void) req.str();
        return http::Responce("HTTP/1.1 200 "
                                + http::to_string(http::StatusCode::OK)
                                + "\r\nContent-Length: 76\r\n\r\nHello world! My name is Andrew Sherstobitov"
                                + "And i'm studying at Technosphere!");
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <sizeof(workers)>\n";
        return -1;
    }
    log::init_with_stdout_logger(log::LEVEL::DEBUG);
    TestListener tl;
    while (true) {
        try {
            http::cor::CoroutineService serv({"127.0.0.1", 8080}, &tl, std::stoi(argv[1]), 10ul, 15ul);
            serv.run();
            break;
        } catch (tcp::AddressError& ex) { std::cout << "useaddr" << std::endl; }
    }

    return 0;
}

