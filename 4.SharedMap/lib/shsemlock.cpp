#include "shsemlock.h"

shmem::SemLock::SemLock(Semaphore& s) : _s(s) {
    _s.wait();
}
shmem::SemLock::~SemLock() {
     try { _s.post(); }
     catch (...) {}
}
