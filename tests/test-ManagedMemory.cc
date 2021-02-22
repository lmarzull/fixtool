#include "doctest.h"
#include <ManagedMemory.h>




SCENARIO("ManagedHeap")
{
  GIVEN("A newly created ManagedHeap memory resource")
  {
    std::byte heap[1024];
    ManagedMemory memory_resource{heap, sizeof(heap)};

    THEN("The available size is at least the asked size")
    {
      REQUIRE(memory_resource.available() >= sizeof(heap));
    }

    THEN("The size is equal to the available size")
    {
      REQUIRE(memory_resource.size() >= memory_resource.available());
    }

    THEN("The reserve size is 0")
    {
      REQUIRE(memory_resource.reserved() == 0);
    }

    THEN("std::begin return a pointer to the start of the buffer")
    {
      REQUIRE(std::begin(memory_resource) == std::begin(heap));
    }

    THEN("std::end return a pointer to the end of the buffer")
    {
      REQUIRE(std::end(memory_resource) == std::end(heap));
    }
  }

  GIVEN("A ManagedHeap with a deletor function")
  {
    std::byte heap[1024];
    auto deletor_is_called = false;
    {
      ManagedMemory memory_resource(heap, sizeof(heap),
        [&](std::byte*){
          deletor_is_called = true;
        });
    }

    THEN("The deletor function is called when memory resource is destroyed")
    {
      REQUIRE(deletor_is_called == true);
    }
  }
}


SCENARIO("Allocating memory")
{
  GIVEN("A newly created ManagedHeap memory resource")
  {
    std::unique_ptr<std::byte[]> heap(new std::byte[1024]);
    ManagedMemory memory_resource{heap.get(), 1024};

    WHEN("Allocating space")
    {
      (void)memory_resource.allocate(sizeof(uint64_t), alignof(uint64_t));
      THEN("The reserved size is at least the allocated size")
      {
        CHECK(memory_resource.reserved() >= sizeof(uint64_t));
      }

      THEN("The available size + the reserved size is equal to the size")
      {
        std::size_t sz = memory_resource.available()
                        + memory_resource.reserved();
        CHECK(sz == memory_resource.size());
      }
    }

    WHEN("Allocating multiple memory area")
    {
      std::byte* elt1_ptr = reinterpret_cast<std::byte*>(
          memory_resource.allocate(sizeof(uint64_t), alignof(uint64_t)));

      std::byte* elt2_ptr = reinterpret_cast<std::byte*>(
          memory_resource.allocate(sizeof(uint64_t), alignof(uint64_t)));

      THEN("The second memory area does not overlap with the first one")
      {
        std::byte* after_elt1_ptr = elt1_ptr + sizeof(uint64_t);
        CHECK(elt2_ptr >= after_elt1_ptr);
      }
    }

    WHEN("Allocating memory needing alignement")
    {
      std::byte* elt1_ptr = reinterpret_cast<std::byte*>(
          memory_resource.allocate(sizeof(std::byte), alignof(std::byte)));

      std::byte* elt2_ptr = reinterpret_cast<std::byte*>(
          memory_resource.allocate(sizeof(uint64_t), alignof(uint64_t)));

      THEN("The memory is align")
      {
        void* just_after_elt1 = elt1_ptr + sizeof(std::byte);
        std::size_t available = memory_resource.available();
        std::align(sizeof(uint64_t), alignof(uint64_t), just_after_elt1, available);
        CHECK(just_after_elt1 == elt2_ptr);
      }
    }

    WHEN("Allocating memory with any of the ManagedHeap")
    {
      ManagedMemory copied{memory_resource};

      void* i1 = copied.allocate(sizeof(uint64_t), alignof(uint64_t));
      void* i2 = memory_resource.allocate(sizeof(uint64_t), alignof(uint64_t));

      THEN("Reserved size is the same for both instance")
      {
        CHECK(memory_resource.reserved() == copied.reserved());
      }

      THEN("Available size is the same for both instance")
      {
        CHECK(memory_resource.available() == copied.available());
      }

      THEN("Allocated memory does not overlap")
      {
        std::byte* end_of_i1 = reinterpret_cast<std::byte*>(i1) + sizeof(uint64_t);
        CHECK(i2 >= end_of_i1);
      }
    }

    WHEN("Allocating 0 byte")
    {
      THEN("Throws bad alloc")
      {
        CHECK_THROWS_AS((void)memory_resource.allocate(0), std::bad_alloc);
      }
    }

    WHEN("Allocating too much memory")
    {
      THEN("Throws bad alloc")
      {
        CHECK_THROWS_AS((void)memory_resource.allocate(2*memory_resource.available()),
                        std::bad_alloc);
      }
    }

    WHEN("Not enough space for data type because of alignment")
    {
      std::size_t avail = memory_resource.available();
      (void)memory_resource.allocate(avail-1);
      CHECK(memory_resource.available() == 1);

      THEN("Throws bad alloc")
      {
        CHECK_THROWS_AS((void)memory_resource.allocate(1, 2),
                        std::bad_alloc);
      }
    }
  }
}



#if 0
#include <ManagedHeap.h>
#include <SharedMemory.h>
#include <iostream>


struct ManagedHeapTestTraits
{
  using ManagedMemoryType = ManagedHeap;

  static ManagedMemoryType createMemory(std::size_t sz)
  {
    return std::move(ManagedMemoryType(sz));
  }

  static void cleanupMemory(ManagedHeap& shmem)
  {
  }
};


struct SharedMemoryTestTraits
{
  using ManagedMemoryType = SharedMemory;

  static ManagedMemoryType createMemory(std::size_t sz)
  {
    return std::move(SharedMemory("ManagedMemoryTestSuite", sz));
  }

  static void cleanupMemory(SharedMemory& shmem)
  {
    SharedMemory::unmap(shmem);
    SharedMemory::unlink(shmem);
  }
};


template <typename T_memory_manager_traits>
struct AutoCleanup
{
  using ManagedMemoryType = typename T_memory_manager_traits::ManagedMemoryType;
  AutoCleanup(ManagedMemoryType&& mem)
    : managed_memory(std::move(mem))
  {}


  ~AutoCleanup()
  {
    try {
      T_memory_manager_traits::cleanupMemory(managed_memory);
    }
    catch (...)
    {}
  }

  ManagedMemoryType* operator->() { return &managed_memory; }

  ManagedMemoryType managed_memory;
};


TEST_SUITE("Managed memory_resource") {

  TEST_CASE_TEMPLATE("Free space of ManagedMemory is equal to at least size after construction",
      T, 
      ManagedHeapTestTraits,
      SharedMemoryTestTraits) {

    AutoCleanup<T> mm(std::move(T::createMemory(1024)));
    CHECK(mm->available() == 1024);
  }


  TEST_CASE_TEMPLATE("Fail to create ManagedMemory of size 0",
      T, 
      ManagedHeapTestTraits,
      SharedMemoryTestTraits) {

    REQUIRE_THROWS(T::createMemory(0));
  }


  TEST_CASE_TEMPLATE("ManagedMemory minimum available size is alignof(std::max_align_t)",
      T, 
      ManagedHeapTestTraits,
      SharedMemoryTestTraits) {

    constexpr std::size_t sz = 3;
    static_assert((sz % alignof(std::max_align_t)) != 0, "Bad value. Change sz value to be diffeent from alignof(std::max_align_t)");
    AutoCleanup<T> mm(std::move(T::createMemory(sz)));
    REQUIRE(mm->available() == alignof(std::max_align_t));
  }


  TEST_CASE_TEMPLATE("ManagedMemory available size is alignof(std::max_align_t) when asking alignof(std::max_align_t) bytes",
      T, 
      ManagedHeapTestTraits,
      SharedMemoryTestTraits) {

    constexpr std::size_t sz = alignof(std::max_align_t);
    AutoCleanup<T> mm(std::move(T::createMemory(sz)));
    REQUIRE(mm->available() == sz);
  }


  TEST_CASE_TEMPLATE("ManagedMemory available size is a multiple of alignof(std::max_align_t)",
      T, 
      ManagedHeapTestTraits,
      SharedMemoryTestTraits) {

    constexpr std::size_t sz = 30;
    static_assert((sz % alignof(std::max_align_t)) != 0, "Bad value. Change sz value to be not multiple of alignof(std::max_align_t)");
    constexpr std::size_t expected_size = ((sz/alignof(std::max_align_t))+1)*alignof(std::max_align_t);
    AutoCleanup<T> mm(std::move(T::createMemory(sz)));
    REQUIRE(mm->available() == expected_size);
  }


  TEST_CASE_TEMPLATE("ManagedMemory available size is the given size when the size if a multiple of alignof(std::max_align_t)",
      T, 
      ManagedHeapTestTraits,
      SharedMemoryTestTraits) {

    constexpr std::size_t sz = alignof(std::max_align_t)*4;
    AutoCleanup<T> mm(std::move(T::createMemory(sz)));
    REQUIRE(mm->available() == sz);
  }


  TEST_CASE_TEMPLATE("Allocation of 0 bytes throws",
      T, 
      ManagedHeapTestTraits,
      SharedMemoryTestTraits) {

    AutoCleanup<T> mm(std::move(T::createMemory(10)));
    REQUIRE_THROWS((void)mm->allocate(0));
  }


  TEST_CASE_TEMPLATE("Allocated size is align to the alignment parameter",
      T, 
      ManagedHeapTestTraits,
      SharedMemoryTestTraits) {

    AutoCleanup<T> mm(std::move(T::createMemory(32)));
    std::size_t total_space = mm->available();
    constexpr std::size_t asked_alignment = 8;
    auto* _ = mm->allocate(1, asked_alignment);
    REQUIRE(mm->available() == total_space-asked_alignment);
  }


  TEST_CASE_TEMPLATE("Allocation of a bigger size than the available one throw bad_alloc",
      T, 
      ManagedHeapTestTraits,
      SharedMemoryTestTraits) {

    AutoCleanup<T> mm(std::move(T::createMemory(32)));
    REQUIRE_THROWS_AS((void)mm->allocate(100), std::bad_alloc);
  }


  TEST_CASE_TEMPLATE("Writing to managed memory",
      T, 
      ManagedHeapTestTraits,
      SharedMemoryTestTraits) {

    AutoCleanup<T> mm(std::move(T::createMemory(32)));
    char* dest = reinterpret_cast<char*>(mm->baseAddress());
    *dest = '_';
    CHECK(*dest == '_');
  }
}
#endif
