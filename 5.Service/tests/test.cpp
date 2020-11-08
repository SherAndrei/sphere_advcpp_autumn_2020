#include <iostream>
#include "service.h"
#include "tcperr.h"

class EchoListner : public net::IServiceListener {
    void onNewConnection(net::BufferedConnection* cn) override {
        std::cout << "New client!" << std::endl;
        cn->subscribe(net::OPTION::READ_AND_WRITE);
        (void) cn;
    }
    void onClose(net::BufferedConnection* cn)         override {
        std::cout << "Client disconnected!" << std::endl;
        (void) cn;
    }
    void onWriteDone(net::BufferedConnection* cn)     override {
        std::string buf = "Hello world!";
        cn->write(buf.data(), buf.length());
        std::cout << "Write happened!" << std::endl;
        cn->unsubscribe(net::OPTION::WRITE);
        (void) cn;
    }
    void onReadAvailable(net::BufferedConnection* cn) override {
        std::string buf(1024, '\0');
        cn->read(buf.data(), buf.length());
        std::cout << buf << std::endl;
        cn->unsubscribe(net::OPTION::READ);
    }
    void onError(net::BufferedConnection* cn)         override {
        std::cout << "Error happened!" << std::endl;
        (void) cn;
    }
};

int main() {
    EchoListner el;
    net::Service service(&el);
    try {
        service.open({"127.0.0.1", 8080});
        service.run();
    } catch (tcp::AddressError& ex) {
        std::cout << ex.what() << std::endl;
    }
}

