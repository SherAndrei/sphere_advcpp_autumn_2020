#include "httpservice.h"
#include "httperr.h"
#include "worker.h"

namespace http {

Worker::Worker(HttpService* service)
    : service_(service) {}

void Worker::set_thread(std::thread&& other) {
    thread_ = std::move(other);
}

void Worker::work() {
    while (true) {
        std::vector<::epoll_event> epoll_events = epoll_.wait();
        for (::epoll_event& event : epoll_events) {
            auto it_client = service_->manager_.find(event.data.fd);
            HttpConnection& client = *(dynamic_cast<HttpConnection*>(it_client->get()));
            if (event.events & EPOLLERR) {

            } else if (event.events & EPOLLIN) {
                client.read_to_buffer();
                try {
                    Request req(client.read_buf());
                } catch (ExpectingData& exd) {
                    continue;
                } catch (IncorrectData& ind) {
                    // TODO
                }
                

            } else if (event.events & EPOLLOUT) {
                // if (!client.write_buf().empty()) {
                    // size_t size = client.write_from_buffer();
                    // if (size == 0)
                        // listener_->onError(client);
                // } else {
                    // listener_->onWriteDone(client);
                // }
            }
            // if (client.epoll_option_ == OPTION::UNKNOW ||
                // event.events & EPOLLRDHUP) {
                // listener_->onClose(client);
                // client.close();
                // manager_.erase(it_client);
            // }
        }
    }
}


}  // namespace http
