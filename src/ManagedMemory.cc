#include "ManagedMemory.h"
#include <sstream>
#include <exception>
#include <iostream>


//------------------------------------------------------------------------------
std::size_t
ManagedMemory::MemoryState::available() const
{
  std::byte* upper_bound = base_address + size;
  return upper_bound > free_space ? upper_bound - free_space
                                   : 0;
}


//------------------------------------------------------------------------------
std::size_t
ManagedMemory::MemoryState::reserved() const
{
  return free_space - base_address;
}






//------------------------------------------------------------------------------
ManagedMemory::ManagedMemory(
      std::byte* base_address,
      std::size_t sz,
      std::function<void(std::byte*)> deletor)
  : _mem(new MemoryState{base_address, sz, deletor})
{
}


//------------------------------------------------------------------------------
ManagedMemory::ManagedMemory(const ManagedMemory& mm)
  : _mem(mm._mem)
{
}


//------------------------------------------------------------------------------
ManagedMemory::ManagedMemory(ManagedMemory&& mm)
  : _mem(std::move(mm._mem))
{
}


//------------------------------------------------------------------------------
std::size_t
ManagedMemory::available() const
{
  return _mem->available();
}


//------------------------------------------------------------------------------
std::size_t
ManagedMemory::reserved() const
{
  return _mem->reserved();
}


//------------------------------------------------------------------------------
void*
ManagedMemory::do_allocate(std::size_t size, std::size_t alignment)
{
  if (size == 0)
    throw std::bad_alloc();

  void* ptr_to_free_space = _mem->free_space;
  std::size_t freespace = available();
  std::size_t sz = freespace;

  void* aligned_memory = std::align(alignment, size, ptr_to_free_space, sz);
  const std::size_t bytes_used_to_align = (freespace - sz);
  if (aligned_memory == nullptr)
    throw std::bad_alloc();

  _mem->free_space += (bytes_used_to_align+size);
  return aligned_memory;
}


//-----------------------------------------------------------------------------
void
ManagedMemory::do_deallocate(void* addr, std::size_t size, std::size_t alignment)
{
}


//------------------------------------------------------------------------------
bool
ManagedMemory::do_is_equal(const std::pmr::memory_resource& other ) const noexcept
{
  return dynamic_cast<const ManagedMemory*>(&other) != nullptr;
}
