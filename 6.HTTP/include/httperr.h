#ifndef HTTP_ERROR_H
#define HTTP_ERROR_H
#include <string>
#include <stdexcept>
#include "address.h"

namespace http {

class Error : public std::runtime_error {
 public:
    using std::runtime_error::runtime_error;
};

class ParsingError : public Error {
 public:
    using Error::Error;
};

class WorkerError : public Error {
 public:
    using Error::Error;
};

}  // namespace http

#endif  // HTTP_ERROR_H
