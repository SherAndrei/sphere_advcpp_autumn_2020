#ifndef SHMEM_SEMAPHORE_H
#define SHMEM_SEMAPHORE_H
#include <semaphore.h>

namespace shmem {
namespace sem   {

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

}  // namespace sem
}  // namespace shmem

#endif  // SHMEM_SEMAPHORE_H
