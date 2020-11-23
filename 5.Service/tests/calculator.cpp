#include <iostream>
#include "service.h"
#include "tcperr.h"

class Calculator : public net::IServiceListener {
    int numbers[2];

    void onNewConnection(net::BufferedConnection& cn) override {
        std::cout << "New client " << cn.adress().port() << std::endl;
        cn.subscribe(net::OPTION::READ);
    }
    void onClose(net::BufferedConnection& cn)         override {
        std::cout << "Client " << cn.adress().port() << " disconnected!" << std::endl;
        (void) cn;
    }
    void onReadAvailable(net::BufferedConnection& cn) override {
        std::string buffer;
        cn.read(buffer);
        if (buffer.size() == 2 * sizeof(int)) {
            cn.unsubscribe(net::OPTION::READ);
            for (int i = 0; i < 2; i++)
                numbers[i] = (reinterpret_cast<int*>(buffer.data()))[i];
            int sum = numbers[0] + numbers[1];
            cn.write({reinterpret_cast<char*>(&sum), 4});
            cn.subscribe(net::OPTION::WRITE);
        }
    }
    void onWriteDone(net::BufferedConnection& cn)     override {
        std::cout << "Client a + b: " << numbers[0] + numbers[1] << std::endl;
        cn.unsubscribe(net::OPTION::WRITE);
    }
    void onError(net::BufferedConnection& cn)         override {
        std::cout << "Error occured!" << std::endl;
        int result = -1;
        cn.write({reinterpret_cast<char*>(&result), 4});
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
        } catch (...) { std::cout << "useaddr" << std::endl; }
    }
    service.run();
}
