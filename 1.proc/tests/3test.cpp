#include <fstream>
#include <iostream>
#include <string>

int main() {
    std::ifstream file("./tests/somename.txt");

    if (!file.good()) {
         std::cout << "Failure" << std::endl;
        return -1;
    } else {
        std::cout << "Success" << std::endl;
    }

    std::string data;
    while (std::getline(file, data))
        std::cout << data << '\n';

    file.close();
}
