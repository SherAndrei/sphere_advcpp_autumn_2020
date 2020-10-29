#ifndef SH_MMAP_H
#define SH_MMAP_H

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

#endif // SH_MMAP_H