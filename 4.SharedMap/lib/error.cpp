#include "error.h"

using namespace shmem;

SemaphoreError::SemaphoreError(const std::string& msg)
    : Error(msg) {}

MMapError::MMapError(const std::string& msg)
    : Error(msg) {}