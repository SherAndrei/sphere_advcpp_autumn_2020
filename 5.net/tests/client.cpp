#include <iostream>
#include <random>
#include "tcperr.h"
#include "connection.h"

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> number(-100, 100);
    int arr[2];
    arr[0] = number(gen);
    arr[1] = number(gen);
    int c = 0;

    while (true) {
        try {
            tcp::Connection cn({"127.0.0.1", 8080});
            cn.write(arr, 2 * sizeof(int));
            cn.read(&c, sizeof(int));
            break;
        } catch (tcp::AddressError& err) {
            std::cerr << "cn_ref" << std::endl;
        }
    }
    std::cout << std::boolalpha << ((arr[0] + arr[1]) == c) << std::endl;
}
