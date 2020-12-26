#include <fstream>
#include <iostream>
#include <string>

int main() {
    std::ofstream file("./tests/somename.txt");

    if (!file.good()) {
         std::cout << "Failure" << std::endl;
        return -1;
    } else {
        std::cout << "Success" << std::endl;
    }

    std::string data;

    while (std::getline(std::cin, data))
        file << data << '\n';

    std::cout << data << std::endl;
    file.close();
}
