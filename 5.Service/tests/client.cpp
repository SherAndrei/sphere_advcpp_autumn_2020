#include <iostream>
#include <random>
#include "connection.h"

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> number(-100, 100);
    bool cn_ref = true;
    tcp::Connection cn;
    while (cn_ref) {
        try {
            cn.connect({"127.0.0.1", 8080});
            cn_ref = false;
        } catch (...) {
            std::cerr << "cn_ref" << std::endl;
        }
    }

    int arr[2];
    arr[0] = number(gen);
    arr[1] = number(gen);
    int c = 0;
    std::cout << "a: " << arr[0] << std::endl;
    std::cout << "b: " << arr[1] << std::endl;

    cn.write(arr, 2 * sizeof(int));
    cn.read(&c, sizeof(int));

    std::cout << "a + b: " << c << std::endl;
}
