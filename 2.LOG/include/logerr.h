#ifndef LOG_ERROR_H
#define LOG_ERROR_H
#include <stdexcept>

namespace log {

class InstanceError : public std::runtime_error {
 public:
    using std::runtime_error::runtime_error;
};

}  // namespace log

#endif  // LOG_ERROR_H
