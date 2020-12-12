#ifndef HTTP_COR_ERROR_H
#define HTTP_COR_ERROR_H
#include <stdexcept>

namespace http {
namespace cor {

class Error : public std::runtime_error {
 public:
    using std::runtime_error::runtime_error;
};

class CoroutineError : public Error {
 public:
    using Error::Error;
};

}  // namespace cor
}  // namespace http


#endif  // HTTP_COR_ERROR_H

