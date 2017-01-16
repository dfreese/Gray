add_executable(Gray
    GammaRayTrace.cpp
    GammaRayTrace.h
    LoadDetector.cpp
    LoadDetector.h
    LoadMaterials.cpp
    LoadMaterials.h
    LoadObjFile.cpp
    LoadObjFile.h
    MaterialStack.cpp
    MaterialStack.h
    RayTraceStats.cpp
    RayTraceStats.h
    SceneDescription.cpp
    SceneDescription.h
    main.cpp
)

target_include_directories(Gray PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

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
