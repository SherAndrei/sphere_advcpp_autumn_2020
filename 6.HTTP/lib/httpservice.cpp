#include "httpservice.h"
#include "option.h"
#include "neterr.h"
#include "httperr.h"

namespace http {

HttpService::HttpService(IHttpListener* listener)
    : listener_(listener) {}

void HttpService::setWorkersSize(size_t size) {
    workers_.resize(size);
}

void HttpService::open(const tcp::Address& addr) {
}

// один epoll у сервера
// onnections у сервера
// сервер делает accept
void HttpService::run() {
    if (listener_ == nullptr)
        throw net::ListenerError("Listener was not set");
    if (workers_.size() == 0)
        throw WorkerError("Worker size was not set");
    while (true) {
        std::vector<::epoll_event> epoll_events = epoll_.wait();
        for (::epoll_event& event : epoll_events) {
        }
    }
}


}  // namespace http
