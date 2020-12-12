#ifndef HTTP_I_TIMED_H
#define HTTP_I_TIMED_H
#include <chrono>

namespace http {

using timeout_t    = std::chrono::seconds;
using time_point_t = std::chrono::system_clock::time_point;

inline constexpr size_t ACCEPT_TIMEOUT = 5u;
inline constexpr size_t CONNECTION_TIMEOUT = 20u;
inline constexpr size_t KEEP_ALIVE_CONNECTION_TIMEOUT = 60u;

class ITimed {
 public:
    ITimed();

 public:
    bool is_timed_out(size_t timeo) const;
    void reset_time_of_last_activity();

 protected:
    time_point_t start_;
};

}  // namespace http

#endif  // HTTP_I_TIMED_H
