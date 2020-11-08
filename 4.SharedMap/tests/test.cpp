#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include "shmap.h"
#include "test_runner.h"

void TestBadAlloc() {
    try {
        shmem::SharedMap<int, double> map(shmem::BlockSize{0}, shmem::BlockCount{0});
        ASSERT(true);
    } catch (std::bad_alloc& ex) {
        ASSERT(false);
    }

    try {
        shmem::SharedMap<int, double> map(shmem::BlockSize{0}, shmem::BlockCount{0});
        map.insert(1, 2.);
        ASSERT(false);
    } catch (std::bad_alloc& ex) {
        ASSERT(true);
    }
    try {
        // 48 - размер одной ноды в std::map<int, double>
        shmem::SharedMap<int, double> map(shmem::BlockSize{48}, shmem::BlockCount{1});
        map.insert(1, 2.);
        ASSERT(true);
        map.insert(2, 3.);
        ASSERT(false);
    } catch (std::bad_alloc& ex) {
        ASSERT(true);
    }
}
void TestFork() {
    {
        shmem::SharedMap<int, int> map(shmem::BlockSize{64}, shmem::BlockCount{4});

        int child = ::fork();
        if (child > 0) {
            map.insert(1, 2);
            map[2] = 1;
            ASSERT(map.size() == 2);
        } else {
            ASSERT(map.at(1) == 2);
            ASSERT(map.at(2) == 1);
            map.erase(2);
            std::cout << "Child ";
            return;
        }
        waitpid(child, nullptr, 0);
        ASSERT(map.size() == 1);
    }
    {
        shmem::SharedMap<int, int> map(shmem::BlockSize{64}, shmem::BlockCount{4});

        int child = ::fork();
        if (child == 0) {
            map[2] = 1;
            map.insert(1, 2);
            ASSERT(map.size() == 2);
            sleep(2);
            ASSERT(map.size() == 1);
            std::cout << "Child ";
            return;
        } else {
            sleep(1);
            map.erase(2);
        }
        waitpid(child, nullptr, 0);
    }
}

void TestString() {
    {
        shmem::SharedMap<shmem::ShString, int> map(shmem::BlockSize{256}, shmem::BlockCount{10});
        shmem::CharAlloc aloc{map.CharAlloc()};
        map[{"one", aloc}] = 1;
        map[{"two", aloc}] = 2;
        map[{"one million two hundred twelve thousand and three", aloc}] = 1'212'003;
        map[{"one million one hundred twelve thousand and three", aloc}] = 1'112'003;
        map[{"one million two hundred twelve thousand and six", aloc}] = 1'212'006;
        ASSERT(map.at({"two", aloc}) == 2);
        ASSERT(map.at({"one million two hundred twelve thousand and three", aloc}) == 1'212'003);
        ASSERT(map.at({"one million one hundred twelve thousand and three", aloc}) == 1'112'003);
        ASSERT(map.at({"one million two hundred twelve thousand and six", aloc}) == 1'212'006);
    }
    {
        using shmem::ShString;
        shmem::SharedMap<int, ShString> map(shmem::BlockSize{256}, shmem::BlockCount{10});
        shmem::CharAlloc aloc{map.CharAlloc()};
        map.insert(2, {"two", aloc});
        map.insert(1'212'003, {"one million two hundred twelve thousand and three", aloc});
        map.insert(1'112'003, {"one million one hundred twelve thousand and three", aloc});
        map.insert(1'212'006, {"one million two hundred twelve thousand and six", aloc});
        ASSERT(map.at(2) == ShString("two", aloc));
        ASSERT(map.at(1'212'003) == ShString("one million two hundred twelve thousand and three", aloc));
        ASSERT(map.at(1'112'003) == ShString("one million one hundred twelve thousand and three", aloc));
        ASSERT(map.at(1'212'006) == ShString("one million two hundred twelve thousand and six", aloc));
    }
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestBadAlloc);
    RUN_TEST(tr, TestString);
    RUN_TEST(tr, TestFork);

    return 0;
}
