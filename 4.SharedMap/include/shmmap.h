#ifndef SHMEM_MMAP_H
#define SHMEM_MMAP_H

namespace shmem {

class ShMMap {
 public:
    explicit ShMMap(size_t length);
    ~ShMMap();

    ShMMap(const ShMMap&) = delete;
    ShMMap(ShMMap &&)     = delete;
    ShMMap& operator=(const ShMMap&) = delete;
    ShMMap& operator=(ShMMap&&)      = delete;

    char* get();
 private:
    void* _addr;
    size_t _length;
};

}  // namespace shmem

#endif  // SHMEM_MMAP_H
