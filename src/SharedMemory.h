#ifndef __SHARED_MEMORY_H__
#define __SHARED_MEMORY_H__


#include <filesystem>
#include "OffsetPtr.h"
#include "ManagedMemory.h"


//------------------------------------------------------------------------------
template <typename T>
class SharedMemoryAllocator : public std::pmr::polymorphic_allocator<T>
{
public:
  using value_type = T;
  using size_type = std::size_t;
  using ptrdiff_t = std::ptrdiff_t;

  using pointer = OffsetPtr<value_type>;
  using const_pointer = OffsetPtr<const value_type>;

  using propagate_on_container_copy_assignment = std::true_type;
  using propagate_on_container_move_assignment = std::true_type;
  using propagate_on_container_swap            = std::true_type;

  SharedMemoryAllocator(std::pmr::memory_resource* mr)
    : std::pmr::polymorphic_allocator<T>(mr)
  {
  }

  SharedMemoryAllocator<T> select_on_container_copy_construction() const
  {
    return SharedMemoryAllocator<T>(this->resource());
  }
};



//------------------------------------------------------------------------------
class SharedMemory  : public  ManagedMemory
{
public:
  SharedMemory(std::filesystem::path shared_memory_name, size_t nb_bytes);
  /*
  SharedMemory(const SharedMemory&);
  SharedMemory(SharedMemory&& sm);
  */

  void unmap();
  void unlink();


protected:
  std::filesystem::path _name;

  static int _create_shared_memory(const std::filesystem::path& path, std::size_t);
  static std::size_t _actual_shared_memory_size(int fd);
  static int _open_shared_memory_file(const std::filesystem::path& path);
  static void _reserve(int fd, std::size_t sz);
};



//------------------------------------------------------------------------------
class AutoCleanup
{
public:
  AutoCleanup(SharedMemory& shmem)
    : _shmem(shmem)
  {}


  ~AutoCleanup()
  {
    _shmem.unmap();
    _shmem.unlink();
  }

private:
  SharedMemory& _shmem;
};



//------------------------------------------------------------------------------
class AutoUnmap
{
public:
  AutoUnmap(SharedMemory& shmem)
    : _shmem(shmem)
  {}


  ~AutoUnmap()
  {
    _shmem.unmap();
  }

private:
  SharedMemory& _shmem;
};





#if 0


template <typename T>
class std::pointer_traits<SharedMemoryAllocator<T>>
{
public:
  static T* to_address( SharedMemoryAllocator<T> ptr )
  {
    return ptr.get();
  }
};



//------------------------------------------------------------------------------
class auto_unmap_ptr
{
public:
  auto_unmap_ptr(SharedMemory* shmem)
    : _shmem_ptr(shmem)
  {}


  ~auto_unmap_ptr()
  {
    try {
      SharedMemory::unmap(this->operator*());
    }
    catch ( ... )
    {}
  }


  SharedMemory& operator*() { return *_shmem_ptr.get(); }
  SharedMemory* operator->() { return _shmem_ptr.get(); }


private:
  std::unique_ptr<SharedMemory> _shmem_ptr;
};
#endif


#endif // #ifndef __SHARED_MEMORY_H__
