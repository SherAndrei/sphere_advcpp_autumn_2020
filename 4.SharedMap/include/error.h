#ifndef SHMEM_ERROR_H
#define SHMEM_ERROR_H
#include <stdexcept>

namespace shmem
{

class Error : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

class SemaphoreError : public Error
{
public: 
    SemaphoreError(const std::string& msg);
};

class MMapError : public Error
{
public:
    MMapError(const std::string& msg);
};

} // namespace shmem

#endif // SHMEM_ERROR_H