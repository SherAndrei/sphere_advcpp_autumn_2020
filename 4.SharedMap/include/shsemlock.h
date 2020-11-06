#ifndef SHMEM_SEMLOCK_H
#define SHMEM_SEMLOCK_H
#include "shsemaphore.h"

namespace shmem {

class SemLock {
 public:
    explicit SemLock(Semaphore& s);
    ~SemLock();

 private:
    Semaphore& _s;
};

}  // namespace shmem

#endif  // SHMEM_SEMLOCK_H
