add_executable(Gray
    include/Gray/Config.h
    src/Config.cpp
    include/Gray/GammaMaterial.h
    src/GammaRayTrace.cpp
    include/Gray/GammaRayTrace.h
    src/LoadDetector.cpp
    include/Gray/LoadDetector.h
    src/LoadMaterials.cpp
    include/Gray/LoadMaterials.h
    src/LoadObjFile.cpp
    include/Gray/LoadObjFile.h
    src/main.cpp
)

target_include_directories(Gray PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Gray Physics)
target_link_libraries(Gray Sources)
target_link_libraries(Gray Random)
target_link_libraries(Gray Output)
target_link_libraries(Gray KdTree)
target_link_libraries(Gray VrMath)
target_link_libraries(Gray Graphics)
target_link_libraries(Gray PixelArray)
target_link_libraries(Gray OpenGLRender)

find_package(OpenGL REQUIRED)
target_link_libraries(Gray ${OPENGL_gl_LIBRARIES})
find_package(GLUT REQUIRED)
target_link_libraries(Gray ${GLUT_LIBRARIES})
