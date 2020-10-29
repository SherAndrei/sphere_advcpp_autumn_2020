#include "shmap.h"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>

int main()
{
    shmem::SharedMap<int, int> map(8, 8);

    int child = ::fork();
    if(child > 0) {
        auto p = map.insert(1, 2);
        std::cout << ((p.second) ? "Succ" : "Fail") << std::endl;
        std::cout << "Parent:  "<< map.at(1) << std::endl;
    } else {
        try {
        // sleep(2);
        std::cout << "Child:  "<< map.at(1) << std::endl;
        }
        catch (std::out_of_range& e) {
            std::cout << e.what() << std::endl;
        }
        return 0;
    }

    waitpid(child, nullptr, 0);
    return 0;
}