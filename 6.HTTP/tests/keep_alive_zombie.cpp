#include <iostream>
#include <string>
#include "message.h"
#include "httperr.h"
#include "connection.h"

int main() {
    tcp::Connection c({"127.0.0.1", 8080});
    std::string request = "GET first HTTP/1.1\r\nConnection: Keep-Alive\r\n\r\n";
    std::string responce(512, '\0');
    c.write(request.data(), request.length());
    size_t size = c.read(responce.data(), responce.length());
    responce.resize(size);
    try {
        http::Responce res(responce);
        std::cout << res.text() << std::endl;
    } catch (http::ParsingError& ex) {
        std::cout << "Server wrote garbage :\n"
                  << ex.what()
                  << " in "
                  << responce;
    }
    sleep(12);
}