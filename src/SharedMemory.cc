#include "SharedMemory.h"
#include <sstream>
#include <iostream>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


//------------------------------------------------------------------------------
SharedMemory::SharedMemory(
    std::filesystem::path shared_memory_name,
    size_t size)
  : ManagedMemory(nullptr, 0)
  , _name(std::move(shared_memory_name))
{
  int fd = _create_shared_memory(_name, size);

  std::byte* base_address = (std::byte*)mmap(nullptr, size, (PROT_READ|PROT_WRITE), MAP_SHARED, fd, 0);
  if (base_address == (void*)-1) {
    char buffer[128];
    std::ostringstream os;
    os << "Error while mapping the shared memory '" << _name.c_str() << "': "
       << strerror_r(errno, buffer, sizeof(buffer)-1);
    throw std::logic_error(os.str());
  }
  _mem.reset(new MemoryState(base_address, size, [](std::byte* ptr){}));
}


//------------------------------------------------------------------------------
void
SharedMemory::unmap()
{
  int res = munmap(begin(), size());
  char buffer[1024];
  if (res==-1) {
    std::ostringstream os;
    os << "munmap('" << _name.c_str() << "') error: '" 
       << strerror_r(errno, buffer, sizeof(buffer)-1) << '\'';
    throw std::logic_error(os.str());
  }
}


//------------------------------------------------------------------------------
void
SharedMemory::unlink()
{
  int res = shm_unlink(_name.c_str());
  if (res==-1) {
    char buffer[1024];
    std::ostringstream os;
    os << "shm_unlink('" << _name.c_str() << "') error: '"
       << strerror_r(errno, buffer, sizeof(buffer)-1) << '\'';
    throw std::logic_error(os.str());
  }
}


//------------------------------------------------------------------------------
int
SharedMemory::_create_shared_memory(const std::filesystem::path& path, std::size_t sz)
{
  int fd = _open_shared_memory_file(path);
  if (_actual_shared_memory_size(fd) != sz)
    _reserve(fd, sz);
  return fd;
}


//------------------------------------------------------------------------------
int
SharedMemory::_open_shared_memory_file(const std::filesystem::path& path)
{
  int flags = O_CREAT|O_RDWR;
  int fd = shm_open( path.c_str(), flags, 0600 );
  if (fd==-1)
  {
    char buffer[128];
    std::ostringstream os;
    os << "Error while opening shared memory '" << path.c_str() << "': "
       << strerror_r(errno, buffer, sizeof(buffer)-1);
    throw std::logic_error(os.str());
  }
  return fd;
}


//------------------------------------------------------------------------------
void
SharedMemory::_reserve(int fd, std::size_t sz)
{
  if (ftruncate(fd,sz)!=0)
  {
    char buffer[128];
    std::ostringstream os;
    os << "Could not reserve " << sz << " bytes for shared memory: "
       << strerror_r(errno, buffer, sizeof(buffer)-1);
    throw std::logic_error(os.str());
  }
}


//-------------------------------------------------------------------------------
std::size_t
SharedMemory::_actual_shared_memory_size(int fd)
{
  struct stat shm_stat{};
  if (fstat(fd,&shm_stat)!=0)
  {
    char buffer[128];
    std::ostringstream oss;
    oss << "fstat on shared memory file descriptor failure: "
        << strerror_r(errno, buffer, sizeof(buffer)-1);
    throw std::logic_error(oss.str());
  }
  return static_cast<std::size_t>(shm_stat.st_size);
}

#if 0
//------------------------------------------------------------------------------
SharedMemory::SharedMemory(const SharedMemory& sm)
  : ManagedMemory(sm)
  , _name(sm._name)
{}


//------------------------------------------------------------------------------
SharedMemory::SharedMemory(SharedMemory&& sm)
  : ManagedMemory(std::move(sm))
  , _name(std::move(sm._name))
{
}


SharedMemory::~SharedMemory()
{
}



bool
SharedMemory::do_is_equal(const std::pmr::memory_resource& other) const noexcept
{
  const auto* dmm = dynamic_cast<const SharedMemory*>(&other);
  return dmm != nullptr && dmm == this;
}
#endif
