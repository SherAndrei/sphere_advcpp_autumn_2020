#include <iostream>
#include "service.h"


class EchoListner : public net::IServiceListener {
    void onNewConnection(const net::BufferedConnection& cn) override {
        std::cout << "New client!" << std::endl;
        (void) cn;
    }
    void onClose(const net::BufferedConnection& cn)         override {
        std::cout << "Client disconnected!" << std::endl;
        (void) cn;
    }
    void onWriteDone(const net::BufferedConnection& cn)     override {
        (void) cn;
    }
    void onReadAvailable(const net::BufferedConnection& cn) override {
        (void) cn;
    }
    void onError(const net::BufferedConnection& cn)         override {
        (void) cn;
    }
};

int main() {
    EchoListner el;
    net::Service service(&el);
    service.open({"127.0.0.1", 8080});
    service.run();
}

