#ifndef SHMAP_H
#define SHMAP_H
#include <map>
#include <utility>
#include <functional>
#include <memory>
#include "shsemlock.h"
#include "shmmap.h"
#include "shallocator.h"
#include "shsemaphore.h"

namespace shmem
{

template<class Key, class Value>
using ShPairAlloc = ShAlloc< std::pair<const Key, Value>>;

template<class Key, class Value>
using ShMap = std::map<Key, Value, std::less<Key>, ShPairAlloc<Key, Value>>;

template<class Key, class Value>
class SharedMap
{
public:
    SharedMap(size_t block_size, size_t block_count) 
        : mmap_(sizeof(Semaphore) + sizeof(ShMemState) + block_count + sizeof(ShMap<Key, Value>) + block_size * block_count)
        , s_(new (mmap_.get()) Semaphore)
    {
        // sizeof(Semaphore)               sizeof(ShMap<Key, Value>) 
        //        ||    sizeof(ShMemState)            ||
        //        \/          \/      block_count     \/
        //  | Semaphore | ShMemState | BlockTable |  ShMap  |   Blocks  |
        //  |___________|____________|____________|_________|__|__|__|__|
        ShMemState* sh_state = new (mmap_.get() + sizeof(Semaphore)) ShMemState{};
        sh_state->block_size   = block_size;
        sh_state->blocks_count = block_count;
        sh_state->used_blocks_table = mmap_.get() + sizeof(Semaphore) + sizeof(ShMemState);
        ::memset(sh_state->used_blocks_table, FREE_BLOCK, sh_state->blocks_count);

        ShAlloc<ShMap<Key, Value>> alloc{sh_state};
        p_map_ = new (mmap_.get() + sizeof(Semaphore) + sizeof(ShMemState) + block_count) ShMap<Key, Value>{alloc};
        
        sh_state->first_block = sh_state->used_blocks_table + sh_state->blocks_count + sizeof(ShMap<Key, Value>);
    }
    ~SharedMap() 
    {
        s_->~Semaphore();
        p_map_->~map();
    }
    auto operator[](const Key& k){
        SemLock sl(*s_);
        return p_map_->operator[](k);
    }
    auto insert(const Key& k, const Value& v) {
        SemLock sl(*s_);
        return p_map_->insert({k, v});
    }
    auto at(const Key& k) const {
        SemLock sl(*s_);
        return p_map_->at(k);
    }
    auto erase(const Key& k) {
        SemLock sl(*s_);
        p_map_->erase(k);
    }
private:
    ShMMap mmap_;
    Semaphore* s_;
    ShMap<Key, Value> * p_map_;
};

} // namespace shmem

#endif // SHAREDMAP_H