#ifndef SHMEM_SEMAPHORE_H
#define SHMEM_SEMAPHORE_H
#include <semaphore.h>

namespace shmem {

class Semaphore {
 public:
    Semaphore();
    ~Semaphore();
    void post();
    void wait();
    void destroy();

    Semaphore(const Semaphore&) = delete;
    Semaphore(Semaphore&&)      = delete;
    Semaphore &operator=(const Semaphore&) = delete;
    Semaphore &operator=(Semaphore&&)      = delete;

 private:
    sem_t _sem;
};

}  // namespace shmem

#endif  // SHMEM_SEMAPHORE_H
