
find_package(OpenGL)
find_package(GLUT)

if (OPENGL_FOUND AND OPENGL_GLU_FOUND AND GLUT_FOUND)
add_executable(gray-view
    gray-view.cpp
    GlutRenderer.cpp
    GlutRenderer.h
    Viewer.cpp
    Viewer.h
)
target_include_directories(gray-view PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(gray-view ${OPENGL_LIBRARIES})
target_link_libraries(gray-view ${GLUT_LIBRARIES})
target_link_libraries(gray-view gammaray)

endif (OPENGL_FOUND AND OPENGL_GLU_FOUND AND GLUT_FOUND)
