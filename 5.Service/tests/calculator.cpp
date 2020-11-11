#include <iostream>
#include "service.h"
#include "tcperr.h"

class Calculator : public net::IServiceListener {
    int numbers[2];

    void onNewConnection(net::BufferedConnection* cn) override {
        std::cout << "New client" << std::endl;
        cn->subscribe(net::OPTION::READ_AND_WRITE);
    }
    void onClose(net::BufferedConnection* cn)         override {
        std::cout << "Client disconnected!" << std::endl;
        (void) cn;
    }
    void onReadAvailable(net::BufferedConnection* cn) override {
        cn->read(numbers, 2 * sizeof(int));
        std::cout << "Client a: " << numbers[0] << std::endl;
        std::cout << "Client b: " << numbers[1] << std::endl;
        cn->unsubscribe(net::OPTION::READ);
        int result = numbers[0] + numbers[1];
        cn->write(&(result), sizeof(int));
        std::cout << "Client a + b: " << result << std::endl;
    }
    void onWriteDone(net::BufferedConnection* cn)     override {
        cn->unsubscribe(net::OPTION::WRITE);
    }
    void onError(net::BufferedConnection* cn)         override {
        int result = -1;
        cn->write(&result, sizeof(int));
    }
};

int main() {
    Calculator el;
    net::Service service(&el);
    service.open({"127.0.0.1", 8080});
    service.run();
}
