#include <iostream>
#include "httpservice.h"

class TestListener : public http::IHttpListener {
    void OnRequest(http::HttpConnection& cn) override {
        std::cout << cn.adress() << std::endl;
    }
};

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <sizeof(workers)>\n";
        // return -1;
    }

    TestListener tl;
    http::HttpService serv(&tl);

    serv.open({"127.0.0.1", 8080});
    serv.run();

    return 0;
}

