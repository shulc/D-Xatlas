cmake_minimum_required(VERSION 3.16)
project(d_xatlas CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(BUILD_SHARED_LIBS OFF)

# ---- Static library: xatlas + our C shim, all in one archive ---------------
add_library(xatlas_c STATIC
    extern/xatlas/source/xatlas/xatlas.cpp
    csrc/xatlas_c.cpp
)

target_include_directories(xatlas_c
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extern/xatlas/source/xatlas
)

# xatlas is header-only C++11/14, no extra deps needed.
target_compile_options(xatlas_c PRIVATE -Wall -Wno-unused-function)
