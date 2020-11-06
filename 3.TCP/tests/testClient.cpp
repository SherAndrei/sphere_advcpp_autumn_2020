#include "test_runner.h"
#include "connection.h"
#include "server.h"
#include "address.h"
#include "tcperr.h"

void TestConnect() {
    try {
        tcp::Connection c;
        c.connect({"something", 8080});
        ASSERT(false);
    } catch (tcp::AddressError& ex) {
        std::string exep = "Incorrect address!";
        ASSERT(ex.what() == exep);
    }

    try {
        tcp::Connection c;
        c.connect({"127.0.0.1", 1000});
        ASSERT(false);
    } catch (tcp::AddressError& ex) {
        std::string exep = "Connection refused";
        ASSERT(ex.what() == exep);
    }
}

void TestClient() {
    try {
        tcp::Connection con;
        con.connect({"127.0.0.1", 8080});

        std::string text = "Studying sphere!\n";

        con.write(text.data(), text.length());
        std::cout << "Success?" << std::endl;

        std::string buf(512, '\0');
        con.read(buf.data(), buf.length());
        std::cout << buf << std::endl;
    } catch (...) {
        std::cout << "Fail!" << std::endl;
    }
}

int main() {
    TestRunner t;

    RUN_TEST(t, TestConnect);
    RUN_TEST(t, TestClient);
}
