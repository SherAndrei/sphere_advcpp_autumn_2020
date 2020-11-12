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
    explicit SemaphoreError(const std::string& msg);
};

class MMapError : public Error {
 public:
    explicit MMapError(const std::string& msg);
};

}  // namespace shmem

#endif  // SHMEM_ERROR_H
