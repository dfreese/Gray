add_library(PixelArray
    src/PixelArray.cpp
    include/PixelArray/PixelArray.h
    src/OpenGlRgbImage.cpp
    include/PixelArray/OpenGlRgbImage.h
    src/Viewer.cpp
    include/PixelArray/Viewer.h
)

target_include_directories(PixelArray PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(PixelArray KdTree)
target_link_libraries(PixelArray Graphics)
target_link_libraries(PixelArray GraphicsTrees)
target_link_libraries(PixelArray VrMath)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(PixelArray ${OPENGL_LIBRARIES})
find_package(GLUT REQUIRED)
target_link_libraries(PixelArray ${GLUT_LIBRARIES})
