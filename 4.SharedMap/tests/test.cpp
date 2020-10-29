#include "shmap.h"
#include "test_runner.h"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>

void TestBadAlloc()
{
    using namespace shmem;
    try {
        SharedMap<int, double> map(BlockSize{0}, BlockCount{0});
        ASSERT(true);
    } catch (std::bad_alloc& ex) {
        ASSERT(false);
    }

    try {
        SharedMap<int, double> map(BlockSize{0}, BlockCount{0});
        map.insert(1, 2.);
        ASSERT(false);
    } catch (std::bad_alloc& ex) {
        ASSERT(true);
    }
    try {
        // 48 - размер одной ноды в std::map<int, double>
        SharedMap<int, double> map(BlockSize{48}, BlockCount{1});
        map.insert(1, 2.);
        ASSERT(true);
        map.insert(2, 3.);
        ASSERT(false);
    } catch (std::bad_alloc& ex) {
        ASSERT(true);
    }
}
void TestFork()
{
    using namespace shmem;
    {
        SharedMap<int, int> map(BlockSize{64}, BlockCount{4});

        int child = ::fork();
        if(child > 0) {
            map.insert(1, 2);
            map[2] = 1;
            ASSERT(map.size() == 2);
        } else {
            ASSERT(map.at(1) == 2);
            ASSERT(map.at(2) == 1);
            map.erase(2);
            return;
        }
        waitpid(child, nullptr, 0);
        ASSERT(map.size() == 1);
    }
    {
        SharedMap<int, int> map(BlockSize{64}, BlockCount{4});

        int child = ::fork();
        if(child == 0) {
            map.insert(1, 2);
            map[2] = 1;
            ASSERT(map.size() == 2);
            sleep(1);
            ASSERT(map.size() == 1);
            return;
        } else {
            sleep(1);
            map.erase(2);
        }
        waitpid(child, nullptr, 0);
    }
    
}

int main()
{

    TestRunner tr;
    RUN_TEST(tr, TestBadAlloc);
    RUN_TEST(tr, TestFork);

    return 0;
}