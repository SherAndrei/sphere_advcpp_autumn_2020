#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <vector>
#include "test_runner.h"
#include "process.h"

void TestWrite();
void TestWriteExact();
void TestReadExact();
void TestParse();
void TestIsRunning();
void TestOpenClose();

void TestWrite() {
    size_t size;
    // creates a file "./tests/somename.txt"
    try {
    prc::Process proc("./tests/1test");
    std::string str = "somename.txt\n";
        size = proc.write(str.data(), str.length());
        ASSERT_EQUAL(size, 13u);

        std::string file_cond(8, '\0');
        proc.read(file_cond.data(), file_cond.length());
        ASSERT_EQUAL(file_cond, "Success\n");
        } catch (std::runtime_error& ex) {
        std::cerr << ex.what() << std::endl;
        ASSERT(false);
    }
}

void TestWriteExact() {
    // writesExact to a file with file name "somename" str
    try {
        prc::Process proc("./tests/2test");
        std::string file_cond(8, '\0');
        // читаем состояние файла
        proc.read(file_cond.data(), file_cond.length());
        ASSERT_EQUAL(file_cond, "Success\n");

        // строчку которую мы отправим
        std::string str = "Studying C++\n at \nSphere!";

        // все хорошо, пишется str.length()
        proc.writeExact(str.data(), str.length());
        // дескриптор становится невалидным
        proc.closeStdin();

        // даем дочерернему процессу успеть записать слова в файл
        ::sleep(1);

        str.clear();
        str.resize(7);
        // считываем последнее слово
        proc.read(str.data(), str.length());
        // проверяем
        ASSERT_EQUAL(str, "Sphere!");
    } catch (std::runtime_error& ex) {
        std::cerr << ex.what() << std::endl;
        ASSERT(false);
    }
}

void TestReadExact() {
    // readExact слова из файла "./tests/somename.txt"
    try {
        prc::Process proc("./tests/3test");
        std::string file_cond(8, '\0');
        // читаем состояние файла
        proc.read(file_cond.data(), file_cond.length());
        ASSERT_EQUAL(file_cond, "Success\n");

        std::string str(10, '\0');
        proc.readExact(str.data(), 10);
        ASSERT_EQUAL(str, "Studying C");
    } catch (std::runtime_error& ex) {
        std::cerr << ex.what() << std::endl;
        ASSERT(false);
    }
}

void TestParse() {
    try {
    {
        prc::Process pr("/usr/bin/echo", {"word1", "word2"});

        size_t size;
        std::string s_word(5, '\0');
        size = pr.read(s_word.data(), s_word.length());
        ASSERT_EQUAL(size, 5u);
        ASSERT_EQUAL(s_word, "word1");
        size = pr.read(s_word.data(), s_word.length());
        ASSERT_EQUAL(size, 5u);
        ASSERT_EQUAL(s_word, " word");
    }
    {
        prc::Process proc("/usr/bin/tr", {"[a-z]", "[A-Z]"});
        std::string word = "some word\n";
        proc.write(word.data(), word.length());
        proc.closeStdin();

        word.clear();
        word.resize(10);

        proc.read(word.data(), word.length());
        ASSERT_EQUAL(word, "SOME WORD\n");
    }
    } catch (std::runtime_error& ex) {
        std::cerr << ex.what() << std::endl;
        ASSERT(false);
    }
}

void TestIsRunning() {
    prc::Process pr("./tests/1test");
    ASSERT_EQUAL(pr.isRunning(), true);
    pr.close();
    ASSERT_EQUAL(pr.isRunning(), false);
}

void TestOpenClose() {
    try {
        prc::Process pr("./tests/1test");
        try {
            pr.open("./tests/1test");
            ASSERT(false);
        } catch (...) {
            ASSERT(true);
        }
        pr.close();
        try {
            pr.close();
            ASSERT(false);
        } catch (...) {
            ASSERT(true);
        }
        pr.open("/usr/bin/echo", {"testing.."});
        size_t size;
        std::string s_word(9, '\0');
        size = pr.read(s_word.data(), s_word.length());
        ASSERT_EQUAL(size, 9u);
        ASSERT_EQUAL(s_word, "testing..");
        pr.close();
    } catch (std::runtime_error& ex) {
        std::cerr << ex.what() << std::endl;
    }
}

int main() {
    TestRunner tr;

    RUN_TEST(tr, TestWrite);
    RUN_TEST(tr, TestWriteExact);
    RUN_TEST(tr, TestReadExact);
    RUN_TEST(tr, TestParse);
    RUN_TEST(tr, TestIsRunning);
    RUN_TEST(tr, TestOpenClose);
}
