#include <sstream>
#include "globallogger.h"
#include "httpservice.h"
#include "httperr.h"
#include "tcperr.h"
#include "worker.h"

#define TRY_UNTIL_EAGAIN(x)            \
for(;;) {                              \
    try {                              \
        if (x() == 0)                  \
            break;                     \
    } catch (tcp::TimeOutError& ex) {  \
        break;                         \
    }                                  \
}

namespace http {

Worker::Worker(HttpService* service, size_t id)
    : service_(service)
    , id_(id) {}

void Worker::set_thread(std::thread&& other) {
    thread_ = std::move(other);
}

void Worker::join() {
    thread_.join();
}

std::string Worker::info() const {
    return std::to_string(id_);
}

void Worker::work() {
    while (true) {
        log::debug("Worker " + info() + " waits");
        std::vector<::epoll_event> epoll_events = service_->connection_epoll_.wait();
        log::debug("Worker " + info() + " got " + std::to_string(epoll_events.size()) + " new epoll events");
        for (::epoll_event& event : epoll_events) {
            HttpConnection* p_client = static_cast<HttpConnection*>(event.data.ptr);

            if (p_client == nullptr) {
                log::error("Cannot find p_client");
            } else if (event.events & EPOLLIN) {
                log::debug("Worker " + info() + " encounters EPOLLIN from " + p_client->address().str());
                TRY_UNTIL_EAGAIN(p_client->read_to_buffer);
                try {
                    p_client->req_.parse(p_client->read_buf());
                } catch (ExpectingData& exd) {
                    log::warn("Worker " + info()
                                        + " got incomplete request from "
                                        + p_client->address().str());
                    service_->subscribe(*p_client, net::OPTION::READ);
                    continue;
                } catch (IncorrectData& ind) {
                    log::warn("Worker " + info()
                                        + " got incorrect request from "
                                        + p_client->address().str());
                    p_client->close();
                    continue;
                }
                // TODO: LOCK
                service_->listener_->OnRequest(*p_client);
                if (!p_client->is_keep_alive())
                    p_client->unsubscribe(net::OPTION::READ);
                else
                    p_client->read_.clear();
                p_client->subscribe(net::OPTION::WRITE);
            } else if (event.events & EPOLLOUT) {
                // SEGFAULT
                log::debug("Worker " + info() + " encounters EPOLLOUT from " + p_client->address().str());
                TRY_UNTIL_EAGAIN(p_client->write_from_buffer);
                if (p_client->write_buf().empty())
                    p_client->unsubscribe(net::OPTION::WRITE);
            } else {
                if (event.events & EPOLLRDHUP) {
                    log::debug("Worker " + info()
                             + " encounter EPOLLRDHUP from "
                             + p_client->address().str());
                } else if (event.events & EPOLLERR) {
                    log::error("Worker " + info()
                             + " encounters EPOLLERR from "
                             + p_client->address().str());
                }
                p_client->close();
                continue;
            }
            service_->subscribe(*p_client, p_client->epoll_option_);
        }
    }
    log::info("Worker " + info() + " finished");
}


}  // namespace http
