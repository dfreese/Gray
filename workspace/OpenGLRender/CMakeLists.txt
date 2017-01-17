add_library(OpenGLRender
    src/GlutRenderer.cpp
    include/OpenGLRender/GlutRenderer.h
)

target_include_directories(OpenGLRender PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_link_libraries(OpenGLRender Graphics)
target_link_libraries(OpenGLRender DataStructs)
target_link_libraries(OpenGLRender VrMath)
target_link_libraries(OpenGLRender Physics)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(OpenGLRender ${OPENGL_LIBRARIES})
