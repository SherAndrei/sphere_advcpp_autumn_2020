#include <netinet/ip.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/socket.h>

static void on_error(int er) {
    if (er == 0)
        return;
    std::cerr << std::strerror(errno) << std::endl;
    std::exit(1);
}

static void client() {
    int res;
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in sock_addr{};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = ::htons(8080);
    res = ::inet_aton("127.0.0.1", &sock_addr.sin_addr);
    on_error(res == 0);
    
    res = ::connect(fd, reinterpret_cast<sockaddr*>(&sock_addr), sizeof(sock_addr));
    // блокируемся и ждем
    
    std::string buf(1024, '\0');
    size_t read = 0;
    do {
        read = ::read(fd, buf.data(), buf.size());
        std::cout << buf.substr(0, read) << std::flush;
    } while (read > 0);

    on_error(::close(fd));
}

int main() {
    client();
    return 0;
}