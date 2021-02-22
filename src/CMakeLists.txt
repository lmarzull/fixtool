#-------------------------------------------------------------------------------
CMAKE_MINIMUM_REQUIRED(VERSION 3.18)

#FIND_PACKAGE(XercesC REQUIRED)

SET(CMAKE_BUILD_TYPE Debug)

ADD_LIBRARY(fixtool-lib
  ManagedMemory.cc
  SharedMemory.cc
)
#ManagedHeap.cc)
TARGET_LINK_LIBRARIES(fixtool-lib rt)

#ADD_EXECUTABLE(fixtool
#main.cc
#Application.cc
#DOMTreeErrorReporter.cc
#FixtoolParse.cc)

#INCLUDE_DIRECTORIES(fixtool ${XercesC_INCLUDE_DIR})
#TARGET_LINK_LIBRARIES(fixtool XercesC::XercesC stdc++fs fixtool-lib)
