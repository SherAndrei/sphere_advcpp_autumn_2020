#ifndef SHMEM_MAP_H
#define SHMEM_MAP_H
#include <map>
#include <string>
#include <utility>
#include <functional>
#include <type_traits>
#include <memory>
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
    template<typename U>
    using IsString    = std::conditional_t<std::is_same_v<U, std::string>, shmem::ShString, U>;

    using key_type       = IsString<Key>;
    using mapped_type    = IsString<T>;
    using value_type     = std::pair<const key_type, mapped_type>;
    using allocator_type = ShAlloc<value_type>;

    using ShMap       = std::map<key_type, mapped_type, std::less<key_type>, allocator_type>;

 private:
    template<typename U>
    IsString<U> convert(const U &obj) const {
        if constexpr (std::is_same_v<U, std::string>)
            return ShString(obj, p_map_->get_allocator());
        else
            return obj;
    }

 public:
    SharedMap(BlockSize block_size, BlockCount block_count)
        : mmap_(sizeof(Semaphore) + sizeof(ShMemState)
              + block_count.value + sizeof(ShMap)
              + block_size.value * block_count.value)
        , s_(new (mmap_.get()) Semaphore{}) {
        sh_state_ = new (mmap_.get() + sizeof(Semaphore)) ShMemState{};
        sh_state_->block_size   = block_size.value;
        sh_state_->blocks_count = block_count.value;
        sh_state_->used_blocks_table = mmap_.get()
                                    + sizeof(Semaphore)
                                    + sizeof(ShMemState);
        ::memset(sh_state_->used_blocks_table, FREE_BLOCK, sh_state_->blocks_count);

        ShAlloc<ShMap> alloc{sh_state_};
        p_map_ = new (mmap_.get()
                      + sizeof(Semaphore)
                      + sizeof(ShMemState)
                      + sh_state_->blocks_count) ShMap{alloc};

        sh_state_->first_block = sh_state_->used_blocks_table
                               + sh_state_->blocks_count
                               + sizeof(ShMap);
    }
    ~SharedMap() = default;

    void destroy() {
        p_map_->~map();
        s_->~Semaphore();
    }

 public:
    auto get_allocator() const {
        SemLock sl(*s_);
        return p_map_->get_allocator();
    }

 public:
    size_t count(const Key& k) const {
        SemLock sl(*s_);
        return p_map_->count(convert(k));
    }

    void update(const Key& key, const T& val) {
        SemLock sl(*s_);
        p_map_->at(convert(key)) = convert(val);
    }

    bool insert(const std::pair<Key, T>& v) {
        SemLock sl(*s_);
        return (p_map_->insert({convert(v.first), convert(v.second)})).second;
    }

    mapped_type at(const Key& k) const {
        SemLock sl(*s_);
        return p_map_->at(convert(k));
    }

    size_t erase(const Key& k) {
        SemLock sl(*s_);
        return p_map_->erase(convert(k));
    }

    size_t size() const {
        SemLock sl(*s_);
        return p_map_->size();
    }

 private:
    ShMMap mmap_;
    Semaphore* s_;
    ShMemState* sh_state_;
    ShMap* p_map_;
};

}  // namespace shmem

#endif  // SHMEM_MAP_H
