#include <memory>
#include <array>
#include <functional>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <iostream>
#include <thread>
#include <chrono>
// работает только для типов, хранящих свои данные на стеке
template<typename T>
using ShUniquePtr = std::unique_ptr<T, std::function<void(T*)>>;

template<typename T>
ShUniquePtr<T> make_shmem() {
    void* mmap = ::mmap(nullptr, sizeof(T),
                        PROT_WRITE | PROT_READ,
                        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(mmap == MAP_FAILED)
        throw std::exception();
    
    return {static_cast<T*>(mmap), [](T* t) { ::munmap(t, sizeof(T)); }};
}


int main()
{
    ShUniquePtr<std::array<int, 5>> sh_ = make_shmem<std::array<int, 5>>();
    *sh_ = {1, 2, 3, 4, 5};

    int child = fork();
    if(child > 0) {
        *sh_ = {2, 3, 4, 1, 2};
    } else {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
        for (auto e : *sh_)
            std::cerr << e << std::endl;
        return 0; 
    }

    waitpid(child, nullptr, 0);
    return 0;
}