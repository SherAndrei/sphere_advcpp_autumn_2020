#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include "shmap.h"
#include "test_runner.h"

void TestBadAlloc();
void TestString();
void TestFork();

void TestBadAlloc() {
    try {
        shmem::SharedMap<int, double> map(shmem::BlockSize{0}, shmem::BlockCount{0});
        ASSERT(true);
        map.destroy();
    } catch (std::bad_alloc& ex) {
        ASSERT(false);
    }

    try {
        shmem::SharedMap<int, double> map(shmem::BlockSize{0}, shmem::BlockCount{0});
        map.insert({1, 2.});
        ASSERT(false);
        map.destroy();
    } catch (std::bad_alloc& ex) {
        ASSERT(true);
    }
    try {
        // 48 - размер одной ноды в std::map<int, double>
        shmem::SharedMap<int, double> map(shmem::BlockSize{48}, shmem::BlockCount{1});
        map.insert({1, 2.});
        ASSERT(true);
        map.insert({2, 3.});
        ASSERT(false);
        map.destroy();
    } catch (std::bad_alloc& ex) {
        ASSERT(true);
    }
}
void TestFork() {
    {
        shmem::SharedMap<int, int> map(shmem::BlockSize{64}, shmem::BlockCount{4});

        int child = ::fork();
        if (child > 0) {
            map.insert({1, 2});
            map.insert({2, 1});
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
        map.destroy();
    }
    {
        shmem::SharedMap<int, int> map(shmem::BlockSize{64}, shmem::BlockCount{4});

        int child = ::fork();
        if (child == 0) {
            map.insert({2, 1});
            map.insert({1, 2});
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
        map.destroy();
    }
}

void TestString() {
    {
        shmem::SharedMap<std::string, int> map(shmem::BlockSize{256}, shmem::BlockCount{10});
        map.insert({"one", 1});
        map.insert({"two", 2});
        map.insert({"one million two hundred twelve thousand and three", 1'212'003});
        map.insert({"one million one hundred twelve thousand and three", 1'112'003});
        map.insert({"one million two hundred twelve thousand and six", 1'212'006});
        ASSERT(map.at("two") == 2);
        ASSERT(map.at("one million two hundred twelve thousand and three") == 1'212'003);
        ASSERT(map.at("one million one hundred twelve thousand and three") == 1'112'003);
        ASSERT(map.at("one million two hundred twelve thousand and six") == 1'212'006);
        map.destroy();
    }
    {
        shmem::SharedMap<int, std::string> map(shmem::BlockSize{256}, shmem::BlockCount{10});
        map.insert({2, "two"});
        map.insert({1'212'003, "one million two hundred twelve thousand and three"});
        map.insert({1'112'003, "one million one hundred twelve thousand and three"});
        map.insert({1'212'006, "one million two hundred twelve thousand and six"});
        ASSERT(map.at(2) == "two");
        ASSERT(map.at(1'212'003) == "one million two hundred twelve thousand and three");
        ASSERT(map.at(1'112'003) == "one million one hundred twelve thousand and three");
        ASSERT(map.at(1'212'006) == "one million two hundred twelve thousand and six");
        map.destroy();
    }
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestBadAlloc);
    RUN_TEST(tr, TestString);
    RUN_TEST(tr, TestFork);
    return 0;
}
