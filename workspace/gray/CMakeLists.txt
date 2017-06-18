add_executable(gray gray.cpp main.cpp)
target_link_libraries(gray gammaray)

if (MPI_CXX_FOUND)
    add_executable(gray-mpi gray-mpi.cpp main.cpp)
    target_link_libraries(gray-mpi gammaray)
endif (MPI_CXX_FOUND)
