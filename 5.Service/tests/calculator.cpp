#include <iostream>
#include "globallogger.h"
#include "service.h"
#include "bufconnection.h"
#include "tcperr.h"

class Calculator : public net::IServiceListener {
    int result;

    void onNewConnection(net::BufferedConnection& cn) override {
        cn.subscribe(net::OPTION::READ);
    }
    void onClose(net::BufferedConnection& cn)         override {
        (void) cn;
    }
    void onReadAvailable(net::BufferedConnection& cn) override {
        std::string buffer;
        cn.read(buffer);
        if (buffer.size() == 2 * sizeof(int)) {
            cn.unsubscribe(net::OPTION::READ);
            result = reinterpret_cast<int*>(buffer.data())[0]
                   + reinterpret_cast<int*>(buffer.data())[1];
            cn.write({reinterpret_cast<char*>(&result), sizeof(int)});
            cn.subscribe(net::OPTION::WRITE);
        }
    }
    void onWriteDone(net::BufferedConnection& cn)     override {
        std::cout << "Client a + b: " << result << std::endl;
        cn.unsubscribe(net::OPTION::WRITE);
    }
    void onError(net::BufferedConnection& cn)         override {
        result = -1;
        cn.write({reinterpret_cast<char*>(&result), sizeof(int)});
    }
};

int main() {
    log::init_with_stderr_logger();
    Calculator el;
    while (true) {
        try {
            net::Service service({"127.0.0.1", 8080}, &el);
            service.run();
        } catch (...) { std::cout << "useaddr" << std::endl; }
    }
}
