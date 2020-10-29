#include "shsemaphore.h"
#include <cstring>
#include "error.h"
#include <semaphore.h>

using namespace shmem;

static void handle_error(int err) 
{ 
    if(err == -1)
        throw SemaphoreError(std::strerror(errno));
}

Semaphore::Semaphore()
{
    handle_error(::sem_init(&(_sem), 1, 1));
}
Semaphore::~Semaphore()
{
    try{ destroy(); }
    catch (const SemaphoreError& er) 
    {}
}
void Semaphore::post()
{
    handle_error(::sem_post(&(_sem)));
}
void Semaphore::wait()
{
    handle_error(::sem_wait(&(_sem)));
}
void Semaphore::destroy()
{
    handle_error(::sem_destroy(&(_sem)));
}