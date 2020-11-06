#include "sherr.h"

shmem::SemaphoreError::SemaphoreError(const std::string& msg)
    : Error(msg) {}

shmem::MMapError::MMapError(const std::string& msg)
    : Error(msg) {}
