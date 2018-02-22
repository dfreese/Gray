find_package(Threads REQUIRED)

add_executable(gray gray.cpp)
target_link_libraries(gray gammaray ${CMAKE_THREAD_LIBS_INIT})
add_custom_command(TARGET gray POST_BUILD 
    COMMAND "${CMAKE_COMMAND}" -E copy  "$<TARGET_FILE:gray>" "${CMAKE_SOURCE_DIR}/../bin/")
