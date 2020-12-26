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


static void server() {
    // listening socket
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    int result;

    // флажок переиспользования сокета
    result = ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
    on_error(result);  

    // Заполняем адр
    sockaddr_in sock_addr{};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = ::htons(8080);
    sock_addr.sin_addr = { ::htonl(INADDR_ANY) }; // слушаем на всех адресах

    // привязываем дескриптор к сокету
    result = ::bind(fd, reinterpret_cast<sockaddr*>(&sock_addr), sizeof(sock_addr));

    // устанавливаем как слушающий
    result = listen(fd, 100);
    on_error(result);  

    socklen_t s = sizeof(sock_addr);
    int client = ::accept(fd ,reinterpret_cast<sockaddr*>(&sock_addr), &s);
    on_error(client == -1);

    std::string buf(1024, '\0');
    size_t read;
    do {
        read = ::read(client, buf.data(), buf.size());
        std::cout << buf.substr(0,read) << std::flush;
    } while (read > 0);

    on_error(::close(fd));
}

int main() {
    server();
    return 0;
}