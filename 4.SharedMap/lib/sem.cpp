#include <cstring>
#include "sem.h"
#include "shmemerr.h"

static void handle_error(int err) {
    if (err == -1)
        throw shmem::SemaphoreError(std::strerror(errno));
}

namespace shmem {
namespace sem   {

Semaphore::Semaphore() {
    handle_error(::sem_init(&(_sem), 1, 1));
}
Semaphore::~Semaphore() {
    try { destroy(); }
    catch (...) {}
}
void Semaphore::post() {
    handle_error(::sem_post(&(_sem)));
}
void Semaphore::wait() {
    handle_error(::sem_wait(&(_sem)));
}
void Semaphore::destroy() {
    handle_error(::sem_destroy(&(_sem)));
}

}  // namespace sem
}  // namespace shmem
