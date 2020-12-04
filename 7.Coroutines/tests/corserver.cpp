#include <iostream>
#include "globallogger.h"
#include "corservice.h"

class TestListener : public http::cor::ICoroutineListener {
    http::Responce OnRequest(const http::Request& req) override {
        std::cout << req.str();
        return http::Responce("HTTP/1.1 200 "
                                + http::to_string(http::StatusCode::OK)
                                + "\r\nContent-Length: 11\r\n\r\nHello world");
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <sizeof(workers)>\n";
        return -1;
    }
    log::init_with_stderr_logger(log::LEVEL::INFO);
    TestListener tl;
    http::cor::CoroutineService serv(&tl, std::stoi(argv[1]));
    bool addr_reus = true;
    while (addr_reus) {
        try {
            serv.open({"127.0.0.1", 8080});
            addr_reus = false;
        } catch (tcp::AddressError& ex) { std::cout << "useaddr" << std::endl; }
    }
    serv.run();

    return 0;
}

