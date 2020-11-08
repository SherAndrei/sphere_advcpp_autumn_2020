#ifndef SHMEM_MAP_H
#define SHMEM_MAP_H
#include <map>
#include <string>
#include <utility>
#include <functional>
#include "shsemlock.h"
#include "shmmap.h"
#include "shallocator.h"
#include "shsemaphore.h"

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
    auto begin() {
        SemLock sl(*s_);
        return p_map_->begin();
    }

    auto cbegin() const {
        SemLock sl(*s_);
        return p_map_->cbegin();
    }

    auto end() {
        SemLock sl(*s_);
        return p_map_->end();
    }

    auto cend() const {
        SemLock sl(*s_);
        return p_map_->cend();
    }

 public:
    auto& operator[](const Key& k) {
        SemLock sl(*s_);
        return p_map_->operator[](k);
    }
    auto& operator[](Key&& key) {
        SemLock sl(*s_);
        return p_map_->operator[](std::move(key));
    }

    auto insert(const Key& k, const T& v) {
        SemLock sl(*s_);
        return p_map_->insert({k, v});
    }
    auto& at(const Key& k) {
        SemLock sl(*s_);
        return p_map_->at(k);
    }
    const auto& at(const Key& k) const {
        SemLock sl(*s_);
        return p_map_->at(k);
    }
    auto erase(const Key& k) {
        SemLock sl(*s_);
        return p_map_->erase(k);
    }
    auto size() const {
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
