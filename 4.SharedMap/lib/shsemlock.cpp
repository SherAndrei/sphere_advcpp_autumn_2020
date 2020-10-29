#include "shsemlock.h"

using namespace shmem;

SemLock::SemLock(Semaphore& s) : _s(s) 
    { _s.wait(); }
SemLock::~SemLock() 
    { _s.post(); }