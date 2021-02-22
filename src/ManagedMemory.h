#ifndef __FIXTOOL_MANAGED_MEMORY_H__
#define __FIXTOOL_MANAGED_MEMORY_H__


#include <memory_resource>
#include <functional>
#include <string_view>

#include <iostream>

//------------------------------------------------------------------------------
class ManagedMemory : public  std::pmr::memory_resource
{
public:
  ManagedMemory(std::byte* base_address,
                std::size_t size,
                std::function<void(std::byte*)> deletor = [](std::byte*){}); 
  ManagedMemory(const ManagedMemory&);
  ManagedMemory(ManagedMemory&&);


  std::size_t available() const;
  std::size_t reserved() const;
  std::size_t size() const
  {
    return _mem->size;
  }

  std::byte* begin()
  {
    return const_cast<std::byte*>(static_cast<const ManagedMemory*>(this)->begin());
  }

  const std::byte* begin() const
  {
    return _mem->base_address;
  }

  std::byte* end()
  {
    return const_cast<std::byte*>(static_cast<const ManagedMemory*>(this)->end());
  }

  const std::byte* end() const
  {
    return _mem->base_address + _mem->size;
  }


protected:
  struct MemoryState
  {
    std::byte* base_address;
    std::byte* free_space;
    std::size_t size;
    std::function<void(std::byte*)> free_memory;


    MemoryState(std::byte* base, std::size_t sz, std::function<void(std::byte*)> deletor )
      : base_address(base)
      , free_space(base)
      , size(sz)
      , free_memory(deletor)
    {}


    ~MemoryState()
    {
      free_memory(base_address);
    }
    std::size_t available() const;
    std::size_t reserved() const;
  };
  std::shared_ptr<MemoryState>  _mem;


  void* do_allocate(std::size_t size, std::size_t alignment ) override;
  void do_deallocate(void* addr, std::size_t size, std::size_t alignment ) override;
  bool do_is_equal(const std::pmr::memory_resource& other) const noexcept;
};


#endif  // #ifndef __FIXTOOL_MANAGED_MEMORY_H__
