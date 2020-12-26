#ifndef SHMEM_ERROR_H
#define SHMEM_ERROR_H
#include <string>
#include <stdexcept>

namespace shmem {

class Error : public std::runtime_error {
 public:
    using std::runtime_error::runtime_error;
};

class SemaphoreError : public Error {
 public:
    using Error::Error;
};

class MMapError : public Error {
 public:
    using Error::Error;
};

}  // namespace shmem

#endif  // SHMEM_ERROR_H
