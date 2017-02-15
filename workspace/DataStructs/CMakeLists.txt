
add_library(DataStructs
    include/DataStructs/Array.h
    src/DoubleRecurse.cpp
    include/DataStructs/DoubleRecurse.h
    include/DataStructs/KdTree.h
    src/KdTree.cpp
    include/DataStructs/ShellSort.h
)

target_include_directories(DataStructs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(DataStructs VrMath)
