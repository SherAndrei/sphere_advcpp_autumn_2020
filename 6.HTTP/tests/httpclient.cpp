#include <iostream>
#include <string>
#include "connection.h"

int main() {
    tcp::Connection c({"127.0.0.1", 8080});
    std::string request = "GET first HTTP/1.1\r\nConnection: keep-alive\r\n\r\n";
    std::string responce(512, '\0');
    c.write(request.data(), request.length());
    c.read(responce.data(), responce.length());
    sleep(2);
    request = "GET second HTTP/1.1\r\n\r\n";
    c.write(request.data(), request.length());
    c.read(responce.data(), responce.length());
    std::cout << responce << std::endl;
}