
add_library(KdTree
    include/KdTree/Array.h
    src/DoubleRecurse.cpp
    include/KdTree/DoubleRecurse.h
    include/KdTree/KdTree.h
    src/KdTree.cpp
)

target_include_directories(KdTree PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(KdTree VrMath)
