#include "doctest.h"
#include <SharedMemory.h>


SCENARIO("Shared memory")
{
  GIVEN("A Shared memory")
  {
    SharedMemory  obj("TestUnit", 1024);
    AutoCleanup cleanup(obj);

    WHEN("Allocating memory")
    {
      auto* int_ptr = reinterpret_cast<std::uint32_t*>(obj.allocate(sizeof(std::uint32_t)));
      THEN("The memory is writtable")
      {
        *int_ptr = 123456;
        CHECK(*int_ptr == 123456);
        *int_ptr = 654321;
        CHECK(*int_ptr == 654321);
      }
    }
  }

  GIVEN("Two shared memory segment with the same name")
  {
    SharedMemory shmem1("TestUnit-1", 1024);
    SharedMemory shmem2("TestUnit-1", 1024);
    AutoCleanup shmem1_cleanup(shmem1);
    AutoUnmap shmem2_cleanup(shmem2);

    WHEN("Allocating in the first")
    {
      auto* int_ptr1 = reinterpret_cast<std::uint32_t*>(shmem1.allocate(sizeof(std::uint32_t)));
      *int_ptr1 = 123456;

      THEN("The allocated size is also visible on the second")
      {
        auto* int_ptr2 = reinterpret_cast<uint32_t*>(std::begin(shmem2));
        REQUIRE(*int_ptr1 == 123456);
        REQUIRE(*int_ptr1 == *int_ptr2);
        *int_ptr2 = 110011;
        REQUIRE(*int_ptr1 == 110011);
      }
    }
  }
}
