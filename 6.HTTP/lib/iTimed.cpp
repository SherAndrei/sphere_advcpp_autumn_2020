#include "iTimed.h"

namespace http {

ITimed::ITimed()
    : start_(std::chrono::system_clock::now()) {}

std::mutex& ITimed::mutex() {
    return timeout_mutex_;
}

bool ITimed::is_timed_out(size_t timeo) const {
    time_point_t now = std::chrono::system_clock::now();
    auto limit = std::chrono::seconds(timeo);
    return (now - start_) > limit;
}

void ITimed::reset_time_of_last_activity() {
    start_ = std::chrono::system_clock::now();
}

}  // namespace http
