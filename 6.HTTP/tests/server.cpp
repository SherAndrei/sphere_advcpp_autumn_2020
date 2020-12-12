#include <iostream>
#include "tcperr.h"
#include "globallogger.h"
#include "httpservice.h"

class TestListener : public http::IHttpListener {
    void OnRequest(http::HttpConnection& cn) override {
        // std::cout << cn.request().str();
        if (cn.is_keep_alive()) {
            cn.write(http::Responce("HTTP/1.1 200 "
                                    + http::to_string(http::StatusCode::OK)
                                    + "\r\nConnection: Keep-Alive\r\nContent-Length: 76\r\n\r\nHello world! My name is Andrew Sherstobitov"
                                    + "And i'm studying at Technosphere!"));
        } else {
            cn.write(http::Responce("HTTP/1.1 200 "
                                    + http::to_string(http::StatusCode::OK)
                                    + "\r\nContent-Length: 76\r\n\r\nHello world! My name is Andrew Sherstobitov"
                                    + "And i'm studying at Technosphere!"));
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <sizeof(workers)>\n";
        return -1;
    }
    log::init_with_stderr_logger(log::LEVEL::DEBUG);
    TestListener tl;
    while (true) {
        try {
            http::HttpService serv({"127.0.0.1", 8080}, &tl, std::stoi(argv[1]), 5ul, 10ul);
            serv.run();
            break;
        } catch (tcp::AddressError& ex) { std::cout << "useaddr" << std::endl; }
    }

    return 0;
}

