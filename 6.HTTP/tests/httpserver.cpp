#include <iostream>
#include "tcperr.h"
#include "globallogger.h"
#include "httpservice.h"

class TestListener : public http::IHttpListener {
    void OnRequest(http::HttpConnection& cn) override {
        std::cout << cn.request().str() << std::endl;
        cn.write(http::Responce("HTTP/1.1 200 Success\r\n\r\n"));
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <sizeof(workers)>\n";
        return -1;
    }
    log::init_with_stderr_logger();
    TestListener tl;
    http::HttpService serv(&tl, std::stoi(argv[1]));
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

