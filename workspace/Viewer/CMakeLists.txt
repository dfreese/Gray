add_library(Viewer
    src/PixelArray.cpp
    include/Viewer/PixelArray.h
    src/OpenGlRgbImage.cpp
    include/Viewer/OpenGlRgbImage.h
    src/Viewer.cpp
    include/Viewer/Viewer.h
)

target_include_directories(Viewer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Viewer KdTree)
target_link_libraries(Viewer Graphics)
target_link_libraries(Viewer GraphicsTrees)
target_link_libraries(Viewer VrMath)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(Viewer ${OPENGL_LIBRARIES})
find_package(GLUT REQUIRED)
target_link_libraries(Viewer ${GLUT_LIBRARIES})
