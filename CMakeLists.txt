cmake_minimum_required (VERSION 2.8.12)
project (Gray)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # https://stackoverflow.com/a/31313226/2465202
    # https://stackoverflow.com/a/39110966/2465202
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
        message(FATAL_ERROR "GCC 4.8 or greater is required")
    endif()
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.3)
        message(FATAL_ERROR "Clang 3.3 or greater is required")
    endif()
endif()

option(ENABLE_LTO "Enable link-time optimization for the compilation" OFF)
option(STATIC_BIN "Build fully staticially-linked binaries" OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if (ENABLE_LTO)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -flto")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -flto")
endif (ENABLE_LTO)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(VERSION_MAJOR 0)
set(VERSION_MINOR 8)
set(VERSION_PATCH 0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_subdirectory(src)
add_subdirectory(test)

