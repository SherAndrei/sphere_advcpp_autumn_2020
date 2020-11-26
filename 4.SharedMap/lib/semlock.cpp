#include "semlock.h"

namespace shmem {
namespace sem   {

SemLock::SemLock(Semaphore& s) : _s(s) {
    _s.wait();
}

SemLock::~SemLock() {
     try { _s.post(); }
     catch (...) {}
}

}  // namespace sem
}  // namespace shmem
