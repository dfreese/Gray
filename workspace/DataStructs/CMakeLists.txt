
add_library(DataStructs
    include/DataStructs/Array.h
    include/DataStructs/CLinkedList.h
    src/DoubleRecurse.cpp
    include/DataStructs/DoubleRecurse.h
    include/DataStructs/ShellSort.h
    include/DataStructs/Stack.h
)

target_include_directories(DataStructs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
