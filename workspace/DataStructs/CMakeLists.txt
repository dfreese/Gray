
add_library(DataStructs
    Array.h
    CLinkedList.h
    DoubleRecurse.cpp
    DoubleRecurse.h
    KdTree.cpp
    KdTree.h
    PQueue.h
    ShellSort.h
    Stack.h
)

target_include_directories(DataStructs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
