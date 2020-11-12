#include <iostream>
#include "service.h"
#include "tcperr.h"

class Calculator : public net::IServiceListener {
    int numbers[2];

    void onNewConnection(net::BufferedConnection* cn) override {
        std::cout << "New client " << cn->adress().port() << std::endl;
        cn->subscribe(net::OPTION::READ_AND_WRITE);
    }
    void onClose(net::BufferedConnection* cn)         override {
        std::cout << "Client " << cn->adress().port() << " disconnected!" << std::endl;
        (void) cn;
    }
    void onReadAvailable(net::BufferedConnection* cn) override {
        cn->read(numbers, 2 * sizeof(int));
        cn->unsubscribe(net::OPTION::READ);
        int result = numbers[0] + numbers[1];
        cn->write(&(result), sizeof(int));
    }
    void onWriteDone(net::BufferedConnection* cn)     override {
        std::cout << "Client a + b: " << numbers[0] + numbers[1] << std::endl;
        cn->unsubscribe(net::OPTION::WRITE);
    }
    void onError(net::BufferedConnection* cn)         override {
        std::cout << "Error occured!" << std::endl;
        int result = -1;
        cn->write(&result, sizeof(int));
    }
};

int main() {
    Calculator el;
    net::Service service(&el);
    bool addr_reus = true;
    while (addr_reus) {
        try {
            service.open({"127.0.0.1", 8080});
            addr_reus = false;
        } catch (...) {}
    }
    service.run();
}
