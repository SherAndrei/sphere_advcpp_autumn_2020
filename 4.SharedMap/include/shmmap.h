#ifndef SHMEM_MMAP_H
#define SHMEM_MMAP_H

namespace shmem
{
 
class ShMMap
{
public:
    ShMMap(size_t length);
    ~ShMMap();

    char* get();
private:
    void* _addr;
    size_t _length;
};

} // namespace shmem

#endif // SHMEM_MMAP_H