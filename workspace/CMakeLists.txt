cmake_minimum_required (VERSION 2.8.12)
project (Gray)

find_package(MPI)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Adapted from http://stackoverflow.com/a/7750816/2465202
set(BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../bin")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../lib")
# First for the generic no-config case
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR})
# Second, for multi-config builds (i.e. Debug, Release, RelWithDebInfo)
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BIN_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${LIB_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${LIB_DIR})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

add_subdirectory(gammaray)
add_subdirectory(gray)
add_subdirectory(daqmodel)
add_subdirectory(Viewer)
