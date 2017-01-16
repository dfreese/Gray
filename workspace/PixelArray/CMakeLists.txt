add_library(PixelArray
    PixelArray.cpp
    PixelArray.h
    OpenGlRgbImage.cpp
    OpenGlRgbImage.h
)

target_include_directories(PixelArray PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(PixelArray Graphics)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(PixelArray ${OPENGL_LIBRARIES})
