add_library(OpenGLRender GlutRenderer.cpp)

target_include_directories(OpenGLRender PUBLIC $(CMAKE_CURRENT_SOURCE_DIR))

target_link_libraries(OpenGLRender DataStructs)
target_link_libraries(OpenGLRender VrMath)

find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
    include_directories($(OPENGL_INCLUDE_DIR))
    target_link_libraries(OpenGLRender $(OPENGL_LIBRARIES))
endif (OPENGL_FOUND)
