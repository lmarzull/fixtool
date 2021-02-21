#include "doctest.h"
#include <OffsetPtr.h>


SCENARIO("OffsetPtr")
{
  GIVEN("A default constructed instance")
  {
    OffsetPtr<int>  a;

    THEN("The instance is equal to nullptr")
    {
      REQUIRE(a == nullptr);
    }

    THEN("std::to_address is equal to nullptr")
    {
      REQUIRE(std::to_address(a) == nullptr);
    }

    WHEN("Creating another instance with explicit nullptr pointer")
    {
      OffsetPtr<int>  b{nullptr};
      THEN("Create a null OffsetPtr instance")
      {
        REQUIRE(b == nullptr);
      }
    }

    WHEN("Creating another instance by copying the null OffsetPtr")
    {
      OffsetPtr<int>  b{a};
      THEN("Create a null OffsetPtr instance")
      {
        REQUIRE(b == nullptr);
      }
    }

    WHEN("Creating another instance by moving the null OffsetPtr")
    {
      OffsetPtr<int>  b{std::move(a)};
      THEN("Create a null OffsetPtr instance")
      {
        REQUIRE(b == nullptr);
      }
    }
  }

  GIVEN("An int")
  {
    int a = 123456;
    WHEN("Constructing an OffsetPtr with the address of the int")
    {
      OffsetPtr<int>  ptr_to_a{&a};

      THEN("The instance is not equal to nullptr")
      {
        REQUIRE(ptr_to_a != nullptr);
      }

      THEN("The OffsetPtr point to the int")
      {
        REQUIRE(*ptr_to_a == 123456);
      }

      THEN("std::to_address point to the int")
      {
        REQUIRE(std::to_address(ptr_to_a) == &a);
      }
    }
  }

  GIVEN("An OffsetPtr pointing to a not null int")
  {
    int a = 123456;
    OffsetPtr<int> ptr_to_a{&a};

    WHEN("Constructing by copy")
    {
      OffsetPtr<int> copy_constructed{ptr_to_a};
      THEN("The instance points to the same int")
      {
        REQUIRE(*copy_constructed == 123456);
      }
    }

    WHEN("Constructing with move")
    {
      OffsetPtr<int> move_constructed{std::move(ptr_to_a)};
      THEN("The instance points to the same int")
      {
        REQUIRE(*move_constructed == 123456);
      }
    }

    WHEN("Affecting nullptr to the instance")
    {
      ptr_to_a = nullptr;
      THEN("nullify the instance")
      {
        REQUIRE(ptr_to_a == nullptr);
      }
    }

    WHEN("Affecting a null OffsetPtr to the instance")
    {
      OffsetPtr<int>  null_instance;
      ptr_to_a = null_instance;

      THEN("nullify the instance")
      {
        REQUIRE(ptr_to_a == nullptr);
      }
    }

    WHEN("Moving a null OffsetPtr to the instance")
    {
      OffsetPtr<int>  null_instance;
      ptr_to_a = std::move(null_instance);

      THEN("nullify the instance")
      {
        REQUIRE(ptr_to_a == nullptr);
      }
    }
  }
}
