#ifndef LOG_ERROR_H
#define LOG_ERROR_H
#include <string>
#include <stdexcept>

namespace log {

class Error : public std::runtime_error {
 public:
    using std::runtime_error::runtime_error;
};

class InstanceError : public Error {
 public:
    using Error::Error;
};

class FileLoggerError : public Error {
    std::string _filename;
 public:
    explicit FileLoggerError(const std::string& msg, const std::string& filename);

    std::string filename() const;
};


}  // namespace log

#endif  // LOG_ERROR_H
