#include <iostream>
#include <random>
#include "connection.h"

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> number(-100, 100);
    tcp::Connection cn({"127.0.0.1", 8080});

    size_t size = 2 * sizeof(int);
    int a = number(gen);
    int b = number(gen);
    int c = 0;
    std::cout << "a: " << a << std::endl;
    std::cout << "b: " << b << std::endl;

    cn.write(&size, sizeof(size_t));
    cn.write(&a, sizeof(int));
    cn.write(&b, sizeof(int));
    cn.read(&c, sizeof(int));

    std::cout << "a + b: " << c << std::endl;
}
