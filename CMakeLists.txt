#-------------------------------------------------------------------------------
CMAKE_MINIMUM_REQUIRED(VERSION 3.18)

PROJECT(
  fixtool VERSION 1.0.0
  DESCRIPTION "FIX (Financial Information Exchange) tool"
  LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

INCLUDE(CTest)
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(tests)
