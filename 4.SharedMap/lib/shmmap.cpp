#include "error.h"
#include "shmmap.h"
#include <cstring>
#include <sys/mman.h>

using namespace shmem;

ShMMap::ShMMap(size_t length)
    : _length(length)
{
    _addr = ::mmap(0, _length, PROT_READ | PROT_WRITE,
                               MAP_ANONYMOUS | MAP_SHARED,
                               -1, 0);
    if(_addr == MAP_FAILED)
        throw MMapError(std::strerror(errno));
}

ShMMap::~ShMMap()
{
    if(::munmap(_addr, _length) == -1)
    { /* ???? */ }
}

char* ShMMap::get()
{
    return static_cast<char*>(_addr);
}