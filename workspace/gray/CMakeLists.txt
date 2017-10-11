add_executable(gray gray.cpp main.cpp)
target_link_libraries(gray gammaray)

if (MPI_CXX_FOUND)
    add_executable(gray-mpi gray-mpi.cpp main.cpp)
    target_link_libraries(gray-mpi gammaray)
endif (MPI_CXX_FOUND)

add_custom_command(TARGET gray POST_BUILD 
    COMMAND "${CMAKE_COMMAND}" -E copy  "$<TARGET_FILE:gray>" "../../bin/")

add_custom_command(TARGET gray-mpi POST_BUILD 
    COMMAND "${CMAKE_COMMAND}" -E copy  "$<TARGET_FILE:gray-mpi>" "../../bin/")
