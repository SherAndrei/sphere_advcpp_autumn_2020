#ifndef HTTP_TIMEOUT_H
#define HTTP_TIMEOUT_H
#include <chrono>

namespace http {

using timeout_t    = std::chrono::seconds;
using time_point_t = std::chrono::system_clock::time_point;

inline constexpr int ACCEPT_TIMEOUT = 5;
inline constexpr int CONNECTION_TIMEOUT = 20;
inline constexpr int KEEP_ALIVE_CONNECTION_TIMEOUT = 60;

}  // namespace http

#endif  // HTTP_TIMEOUT_H
