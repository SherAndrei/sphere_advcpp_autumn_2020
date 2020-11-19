#ifndef SHMEM_ALLOCATOR_H
#define SHMEM_ALLOCATOR_H
#include <cmath>
#include <string>
#include <cstring>

constexpr char USED_BLOCK = '1';
constexpr char FREE_BLOCK = '0';

static size_t get_size_in_blocks(size_t bytes, size_t block_size) {
  float blocks_needed = bytes / static_cast<float>(block_size);
  return std::ceil(blocks_needed);
}

static size_t find_free_blocks(size_t blocks_count,
                        const std::string_view& used_table) {
  std::string pattern(blocks_count, FREE_BLOCK);
  size_t pos = used_table.find(pattern);
  if (pos == std::string::npos) {
    throw std::bad_alloc{};
  }
  return pos;
}

namespace shmem {

struct ShMemState {
  size_t blocks_count;
  size_t block_size;
  char* used_blocks_table;
  char* first_block;
};

template<typename T>
class ShAlloc {
 public:
  typedef T value_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef size_t size_type;

  explicit ShAlloc(ShMemState* state)
    : state_{state} {}

  template<class U>
  ShAlloc(const ShAlloc<U>& other) noexcept
    : state_(other.state()) {}

  pointer allocate(size_type n) {
    if (state_->block_size == 0)
        throw std::bad_alloc{};
    size_t blocks_needed = get_size_in_blocks(sizeof(value_type) * n, state_->block_size);
    std::string_view table{state_->used_blocks_table, state_->blocks_count};
    size_t blocks_pos = find_free_blocks(blocks_needed, table);
    ::memset(state_->used_blocks_table + blocks_pos, USED_BLOCK, blocks_needed);
    return reinterpret_cast<pointer>(state_->first_block + blocks_pos * state_->block_size);
  }

  void deallocate(pointer p, size_type n) noexcept {
    size_t offset = (reinterpret_cast<char*>(p) - state_->first_block) / state_->block_size;
    size_t blocks_count = get_size_in_blocks(sizeof(value_type) * n, state_->block_size);
    ::memset(state_->used_blocks_table + offset, FREE_BLOCK, blocks_count);
  }

  ShMemState* state() const {
      return state_;
  }

 private:
  ShMemState* state_;
};

template <class T, class U>
bool operator==(const ShAlloc<T>&a, const ShAlloc<U>&b) {
  return a.state() == b.state();
}

template <class T, class U>
bool operator!=(const ShAlloc<T>&a, const ShAlloc<U>&b) {
  return a.state() != b.state();
}

}  // namespace shmem

#endif  // SHMEM_ALLOCATOR_H
