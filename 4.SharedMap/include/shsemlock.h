#ifndef SHSEMLOCK_H
#define SHSEMLOCK_H
#include "shsemaphore.h"

namespace shmem
{

class SemLock
{
public:
    SemLock(Semaphore& s);
    ~SemLock();
private:
    Semaphore& _s;
};


} // namespace shmem


#endif // SHSEMLOCK_H