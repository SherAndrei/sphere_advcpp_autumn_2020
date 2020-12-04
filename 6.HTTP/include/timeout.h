#ifndef HTTP_TIMEOUT_H
#define HTTP_TIMEOUT_H
#include <chrono>

namespace http {

using timeout_t    = std::chrono::seconds;
using time_point_t = std::chrono::system_clock::time_point;

inline constexpr size_t ACCEPT_TIMEOUT = 5u;
inline constexpr size_t CONNECTION_TIMEOUT = 20u;
inline constexpr size_t KEEP_ALIVE_CONNECTION_TIMEOUT = 60u;

}  // namespace http

#endif  // HTTP_TIMEOUT_H
