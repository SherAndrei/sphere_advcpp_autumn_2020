#ifndef SHSEMAPHORE_H
#define SHSEMAPHORE_H
#include <semaphore.h>

namespace shmem
{

class Semaphore
{
public:
    Semaphore();
    ~Semaphore();
    void post();
    void wait();
    void destroy();

private:
    sem_t _sem;
};

} // namespace shmem

#endif // SHSEMAPHORE_H