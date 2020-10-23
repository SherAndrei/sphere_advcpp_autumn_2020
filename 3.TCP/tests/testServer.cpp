#include "test_runner.h"
#include "connection.h"
#include "server.h"

void TestServer()
{
    try {
        Server ser;
        ser.listen("127.0.0.1", 8080);
        std::string text = "You are awesome!\n";
        
        while(1) {
            Connection to_client = ser.accept();
            to_client.write(text.data(), text.length());
            std::string buf(512, '\0');
            to_client.read(buf.data(), buf.length());
            std::cout << buf << std::endl;
        }
        std::cout << "Success!" << std::endl;
    } catch (...) {
        std::cout << "Fail!" << std::endl;
    }
}

int main()
{
    TestRunner t;

    RUN_TEST(t, TestServer);
}