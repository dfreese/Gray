add_library(GraphicsTrees
    src/IntersectionKdTree.cpp
    include/GraphicsTrees/IntersectionKdTree.h
    src/ShadowKdTree.cpp
    include/GraphicsTrees/ShadowKdTree.h
)

target_include_directories(GraphicsTrees PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(GraphicsTrees KdTree)
target_link_libraries(GraphicsTrees Graphics)
