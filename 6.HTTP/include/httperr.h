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

class WriteError : public Error {
 public:
    using Error::Error;
};

class ParsingError : public Error {
 public:
    using Error::Error;
};

class ExpectingData : public ParsingError {
 public:
    using ParsingError::ParsingError;
};

class IncorrectData : public ParsingError {
 public:
    using ParsingError::ParsingError;
};

class WorkerError : public Error {
 public:
    using Error::Error;
};

}  // namespace http

#endif  // HTTP_ERROR_H
