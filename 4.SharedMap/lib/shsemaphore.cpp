#include <cstring>
#include "shsemaphore.h"
#include "sherr.h"

static void handle_error(int err) {
    if (err == -1)
        throw shmem::SemaphoreError(std::strerror(errno));
}

shmem::Semaphore::Semaphore() {
    handle_error(::sem_init(&(_sem), 1, 1));
}
shmem::Semaphore::~Semaphore() {
    try { destroy(); }
    catch (...) {}
}
void shmem::Semaphore::post() {
    handle_error(::sem_post(&(_sem)));
}
void shmem::Semaphore::wait() {
    handle_error(::sem_wait(&(_sem)));
}
void shmem::Semaphore::destroy() {
    handle_error(::sem_destroy(&(_sem)));
}
