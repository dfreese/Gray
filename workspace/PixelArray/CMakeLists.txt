add_library(PixelArray
    PixelArray.cpp
)

target_include_directories(PixelArray PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(PixelArray Graphics)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(PixelArray PUBLIC ${OPENGL_LIBRARIES})
