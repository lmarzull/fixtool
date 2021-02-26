#include "doctest.h"
#include <OffsetPtr.h>
#include <cstring>


SCENARIO("OffsetPtr")
{
  GIVEN("A default constructed instance")
  {
    OffsetPtr<int>  a;

    THEN("The instance is equal to nullptr")
    {
      CHECK(a == nullptr);
    }

    THEN("std::to_address is equal to nullptr")
    {
      CHECK(std::to_address(a) == nullptr);
    }

    WHEN("Creating another instance with explicit nullptr pointer")
    {
      OffsetPtr<int>  b{nullptr};
      THEN("Create a null OffsetPtr instance")
      {
        CHECK(b == nullptr);
      }
    }

    WHEN("Creating another instance by copying the null OffsetPtr")
    {
      OffsetPtr<int>  b{a};
      THEN("Create a null OffsetPtr instance")
      {
        CHECK(b == nullptr);
      }
    }

    WHEN("Creating another instance by moving the null OffsetPtr")
    {
      OffsetPtr<int>  b{std::move(a)};
      THEN("Create a null OffsetPtr instance")
      {
        CHECK(b == nullptr);
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
        CHECK(ptr_to_a != nullptr);
      }

      THEN("The OffsetPtr point to the int")
      {
        CHECK(*ptr_to_a == 123456);
      }

      THEN("std::to_address point to the int")
      {
        CHECK(std::to_address(ptr_to_a) == &a);
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
        CHECK(*copy_constructed == 123456);
      }
    }

    WHEN("Constructing with move")
    {
      OffsetPtr<int> move_constructed{std::move(ptr_to_a)};
      THEN("The instance points to the same int")
      {
        CHECK(*move_constructed == 123456);
      }
    }

    WHEN("Affecting nullptr to the instance")
    {
      ptr_to_a = nullptr;
      THEN("nullify the instance")
      {
        CHECK(ptr_to_a == nullptr);
      }
    }

    WHEN("Affecting a null OffsetPtr to the instance")
    {
      OffsetPtr<int>  null_instance;
      ptr_to_a = null_instance;

      THEN("nullify the instance")
      {
        CHECK(ptr_to_a == nullptr);
      }
    }

    WHEN("Moving a null OffsetPtr to the instance")
    {
      OffsetPtr<int>  null_instance;
      ptr_to_a = std::move(null_instance);

      THEN("nullify the instance")
      {
        CHECK(ptr_to_a == nullptr);
      }
    }
  }
}



SCENARIO("OffsetPtr: Affecting null OffsetPtr")
{
  GIVEN("a not null OffsetPtr pointing to a value in the same address space")
  {
    struct S
    {
      int value{};
      OffsetPtr<int> ptr;
    };
    S source_struct;
    source_struct.value = 220772;
    source_struct.ptr = &source_struct.value;

    WHEN("affecting a nulll OffsetPtr to that OffsetPtr and moving the OffsetPtr and the value to another address space")
    {
      OffsetPtr<int> n;
      source_struct.ptr = n;

      char buffer[sizeof(S)];
      S* dest_struct = reinterpret_cast<S*>(buffer);
      memmove(buffer, &source_struct, sizeof(S));

      CHECK(dest_struct->ptr.operator->() == nullptr);
    }
  }
}


SCENARIO("OffsetPtr: Affecting null OffsetPtr rvalue")
{
  GIVEN("a not null OffsetPtr pointing to a value in the same address space")
  {
    struct S
    {
      int value{};
      OffsetPtr<int> ptr;
    };
    S source_struct;
    source_struct.value = 220772;
    source_struct.ptr = &source_struct.value;

    WHEN("Moving a nulll OffsetPtr to that OffsetPtr and moving the OffsetPtr and the value to another address space")
    {
      OffsetPtr<int> n;
      source_struct.ptr = std::move(n);

      char buffer[sizeof(S)];
      S* dest_struct = reinterpret_cast<S*>(buffer);
      memmove(buffer, &source_struct, sizeof(S));

      CHECK(dest_struct->ptr.operator->() == nullptr);
    }
  }
}
