add_executable(Gray
    src/GammaRayTrace.cpp
    include/Gray/GammaRayTrace.h
    src/KdTree.cpp
    include/Gray/KdTree.h
    src/LoadDetector.cpp
    include/Gray/LoadDetector.h
    src/LoadMaterials.cpp
    include/Gray/LoadMaterials.h
    src/LoadObjFile.cpp
    include/Gray/LoadObjFile.h
    src/MaterialStack.cpp
    include/Gray/MaterialStack.h
    src/RayTraceStats.cpp
    include/Gray/RayTraceStats.h
    src/SceneDescription.cpp
    include/Gray/SceneDescription.h
    src/main.cpp
)

target_include_directories(Gray PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Gray Physics)
target_link_libraries(Gray Sources)
target_link_libraries(Gray Random)
target_link_libraries(Gray Output)
target_link_libraries(Gray DataStructs)
target_link_libraries(Gray VrMath)
target_link_libraries(Gray Graphics)
target_link_libraries(Gray PixelArray)
target_link_libraries(Gray OpenGLRender)

find_package(OpenGL REQUIRED)
target_link_libraries(Gray ${OPENGL_gl_LIBRARIES})
find_package(GLUT REQUIRED)
target_link_libraries(Gray ${GLUT_LIBRARIES})
