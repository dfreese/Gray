
# Download and unpack googletest at configure time
configure_file(${CMAKE_SOURCE_DIR}/cmake/gtest_cmake.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build)

add_executable(test_gray
    test_main.cpp
    test_daq.cpp
    test_file.cpp
    test_io.cpp
    test_load.cpp
    test_linear.cpp
    test_mapping.cpp
    test_math.cpp
    test_physics.cpp
    test_random.cpp
    test_source.cpp
    test_syntax.cpp
    test_transform.cpp
    test_viewable.cpp
    )
target_link_libraries(test_gray gammaray)
target_link_libraries(test_gray gtest gtest_main)

add_test(
    NAME gray-unit-tests
    COMMAND test_gray --gtest_output=json
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
