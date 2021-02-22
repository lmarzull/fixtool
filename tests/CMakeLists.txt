#===============================================================================
CMAKE_MINIMUM_REQUIRED(VERSION 3.18)


ADD_EXECUTABLE(test-fixtool
  doctest.cc
  test-OffsetPtr.cc
  test-ManagedMemory.cc
  test-SharedMemory.cc
)

#  test-AVLTree.cc
#  test-ManagedMemory.cc
#  test-AVLTree.cc
#  test-ManagedAllocator.cc

TARGET_INCLUDE_DIRECTORIES(test-fixtool PRIVATE ${CMAKE_SOURCE_DIR}/src)
TARGET_LINK_LIBRARIES(test-fixtool fixtool-lib)
