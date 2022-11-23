cmake_minimum_required (VERSION 2.8)

project ("bsc-m03")

set(CMAKE_C_FLAGS "-std=c99 -O3")
set(CMAKE_CXX_FLAGS "-std=c++14 -O3")

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  add_definitions(-D_FILE_OFFSET_BITS=64)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
endif()

add_executable (bsc-m03 bsc-m03.cpp hutucker/hu-tucker.c libsais/libsais.c libsais/libsais16.c)
