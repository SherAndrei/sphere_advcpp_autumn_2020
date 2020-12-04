#include <iostream>

#include "coroutine.h"

void Name() {
    std::cout << "A";
    http::cor::yield();
    std::cout << "N";
    http::cor::yield();
    std::cout << "D";
    http::cor::yield();
    std::cout << "R";
    http::cor::yield();
    std::cout << "E";
    http::cor::yield();
    std::cout << "W\n";
}


void routine1(size_t init, size_t max) {
    auto id = http::cor::current();
    for (size_t i = init; i <= max; i += 2) {
        std::cout << id << ": " << i << std::endl;
        http::cor::yield();
    }
}

void routine2(size_t init, size_t max) {
    auto id = http::cor::current();
    for (size_t i = init; i < max; ++i) {
        std::cout << id << ": " << i << std::endl;
        http::cor::yield();
    }
}

void routine3() {
    auto id = http::cor::current();
    throw std::runtime_error{"runtime_error from coroutine " + std::to_string(id)};
}

int main() {
    auto id = http::cor::create(Name);

    // for (int i = 0; i < 12; i++) {
    //     http::cor::resume(id);
    // }
    // auto c_id = http::cor::current();

    // std::cout << "routine1" << std::endl;

    // auto id = http::cor::create(routine1, 1, 10);

    // for (size_t i = 0; i <= 10; i += 2) {
    //     std::cout << c_id << ": " << i << std::endl;
    //     http::cor::resume(id);
    // }

    // std::cout << "routine2" << std::endl;

    // id = http::cor::create(routine2, 1, 6);

    // for (size_t i = 0; i <= 10; i += 2) {
    //     std::cout << c_id << ": " << i << std::endl;
    //     http::cor::resume(id);
    // }

    // std::cout << "routine3" << std::endl;

    // try {
    //     http::cor::create_and_run(routine3);
    // }
    // catch (std::exception& e) {
    //     std::cerr << e.what() << std::endl;
    // }

    return 0;
}
