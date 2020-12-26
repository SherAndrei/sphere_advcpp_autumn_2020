#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <memory>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <vector>

namespace {

void throw_on(bool cond, const std::string& what) {
  if (cond)
    throw std::runtime_error{what + " errno: " + std::strerror(errno)};
}

void close_fd(int *fd) {
  ::close(*fd);
  delete fd;
}

using ClosingUPtr = std::unique_ptr<int, decltype(&close_fd)>;

ClosingUPtr create_epoll() {
  int epoll = ::epoll_create(1);
  throw_on(epoll < 0, "Error creating epoll");
  return {new int{epoll}, &close_fd};
}

ClosingUPtr create_server() {
  int server = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  throw_on(server < 0, "Error creating server");
  return {new int{server}, &close_fd};
}

void open_server(int server) {
  ::sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = ::htons(8080);
  addr.sin_addr = { ::htonl(INADDR_LOOPBACK) };
  int result = ::bind(server, reinterpret_cast<::sockaddr*>(&addr),
                      sizeof(addr));
  throw_on(result < 0, "Error binding address to server");
  result = ::listen(server, SOMAXCONN);
  throw_on(result < 0, "Error setting listen to server");
}

void subscribe(int epoll, int fd) {
  ::epoll_event event{};
  event.events = EPOLLIN | EPOLLRDHUP;
  event.data.fd = fd;

  int result = ::epoll_ctl(epoll, EPOLL_CTL_ADD, fd, &event);
  throw_on(result < 0, "Error adding fd to epoll");
}

ClosingUPtr accept_from_server(int server) {
  ::sockaddr_in addr{};
  socklen_t size = sizeof(addr);
  int result = ::accept(server, reinterpret_cast<::sockaddr*>(&addr), &size);
  throw_on(result < 0, "Error accepting new connection");
  return {new int{result}, &close_fd};
}

}  // namespace

int main() {
  ClosingUPtr server = create_server();
  open_server(*server);

  ClosingUPtr epoll = create_epoll();
  subscribe(*epoll, *server);

  std::vector<::epoll_event> event_queue{1024};
  std::vector<ClosingUPtr> active_clients;

  while (true) {
    int events_count = ::epoll_wait(*epoll, event_queue.data(),
                                    event_queue.size(), -1);
    throw_on(events_count < 0, "Error on waiting event");
    auto start = event_queue.begin();
    auto end = event_queue.begin() + events_count;
    for (auto it = start; it != end; ++it) {
      if (it->data.fd == *server) {
        ClosingUPtr new_client{accept_from_server(*server)};
        subscribe(*epoll, *new_client);
        active_clients.push_back(std::move(new_client));
        std::cout << "New client" << std::endl;
      } else if (it->events & EPOLLRDHUP) {
        std::cout << "Disconnected client" << std::endl;
        int closed_client = it->data.fd;
        auto client = std::find_if(active_clients.begin(), active_clients.end(),
                                   [closed_client](const ClosingUPtr& client) {
                                       return *client == closed_client;
                                   });
        active_clients.erase(client);
      } else if (it->events & EPOLLIN) {
        std::string buf(1024, '\0');
        ssize_t read = ::read(it->data.fd, buf.data(), buf.size());
        std::cout << "From client " << it->data.fd
                  << ": " << buf.substr(0, read) << std::endl;
      }
    }
  }

  return 0;
}
