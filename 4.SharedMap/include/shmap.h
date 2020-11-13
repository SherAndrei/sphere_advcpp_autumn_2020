#ifndef SHMEM_MAP_H
#define SHMEM_MAP_H
#include <map>
#include <string>
#include <utility>
#include <functional>
#include "semlock.h"
#include "shmmap.h"
#include "shalloc.h"
#include "sem.h"

namespace shmem {

struct BlockSize {
    size_t value;
    explicit BlockSize(size_t num) : value(num) {}
};

struct BlockCount {
    size_t value;
    explicit BlockCount(size_t num) : value(num) {}
};

using CharAlloc = ShAlloc<char>;
using ShString = std::basic_string<char, std::char_traits<char>, CharAlloc>;

template<class Key, class T>
class SharedMap {
 public:
    using ShPairAlloc = ShAlloc<std::pair<const Key, T>>;
    using ShMap       = std::map<Key, T, std::less<Key>, ShPairAlloc>;

    using key_type       = Key;
    using mapped_type    = T;
    using value_type     = std::pair<const Key, T>;
    using allocator_type = ShPairAlloc;

    using iterator       = typename ShMap::iterator;
    using const_iterator = typename ShMap::const_iterator;

 public:
    SharedMap(BlockSize block_size, BlockCount block_count)
        : mmap_(sizeof(Semaphore) + sizeof(ShMemState)
              + block_count.value + sizeof(ShMap)
              + block_size.value * block_count.value)
        , s_(new (mmap_.get()) Semaphore{}) {
        // sizeof(Semaphore)               sizeof(ShMap<Key, Value>)
        //        ||    sizeof(ShMemState)            ||
        //        \/          \/      block_count     \/     block_size * block_count
        //  | Semaphore | ShMemState | BlockTable |  ShMap  |          Blocks        |
        //  |___________|____________|____________|_________|________________________|
        ShMemState* sh_state = new (mmap_.get() + sizeof(Semaphore)) ShMemState{};
        sh_state->block_size   = block_size.value;
        sh_state->blocks_count = block_count.value;
        sh_state->used_blocks_table = mmap_.get()
                                    + sizeof(Semaphore)
                                    + sizeof(ShMemState);
        ::memset(sh_state->used_blocks_table, FREE_BLOCK, sh_state->blocks_count);

        ShAlloc<ShMap> alloc{sh_state};
        p_map_ = new (mmap_.get()
                      + sizeof(Semaphore)
                      + sizeof(ShMemState)
                      + sh_state->blocks_count) ShMap{alloc};

        sh_state->first_block = sh_state->used_blocks_table
                              + sh_state->blocks_count
                              + sizeof(ShMap);
    }
    ~SharedMap() {
        p_map_->~map();
        s_->~Semaphore();
    }

 public:
    auto get_allocator() const {
        SemLock sl(*s_);
        return p_map_->get_allocator();
    }
    auto CharAlloc() const {
        SemLock sl(*s_);
        shmem::CharAlloc ca{p_map_->get_allocator().state()};
        return ca;
    }

 public:
    size_t count(const Key& k) const {
        SemLock sl(*s_);
        return p_map_->count();
    }
    void update(const key_type& key, const value_type& val) {
        SemLock sl(*s_);
        p_map_->at(key) = val;
    }
    bool insert(const value_type& v) {
        SemLock sl(*s_);
        return (p_map_->insert(v)).second;
    }
    mapped_type at(const key_type& k) const {
        SemLock sl(*s_);
        return p_map_->at(k);
    }
    size_t erase(const key_type& k) {
        SemLock sl(*s_);
        return p_map_->erase(k);
    }
    size_t size() const {
        SemLock sl(*s_);
        return p_map_->size();
    }

 private:
    ShMMap mmap_;
    Semaphore* s_;
    ShMap* p_map_;
};

}  // namespace shmem

#endif  // SHMEM_MAP_H
