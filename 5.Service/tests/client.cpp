#include <iostream>
#include "connection.h"

int main() {
    tcp::Connection cn({"127.0.0.1", 8080});
    std::string data("Hopefully i get it now");
    cn.write(data.data(), data.length());
    std::string str(1024, '\0');
    cn.read(str.data(), str.length());
    std::cout << str << std::endl;
}
