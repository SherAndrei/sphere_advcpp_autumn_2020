#ifndef SHMEM_SEMLOCK_H
#define SHMEM_SEMLOCK_H
#include "sem.h"

namespace shmem {
namespace sem   {

class SemLock {
 public:
    explicit SemLock(Semaphore& s);
    ~SemLock();

    SemLock(SemLock&& other)      = delete;
    SemLock(const SemLock& other) = delete;
    SemLock& operator=(const SemLock& other) = delete;
    SemLock& operator=(SemLock&& other)      = delete;

 private:
    Semaphore& _s;
};

}  // namespace sem
}  // namespace shmem

#endif  // SHMEM_SEMLOCK_H
