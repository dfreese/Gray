add_executable(Gray
    3ds.cpp
    GammaRayTrace.cpp
    LoadDetector.cpp
    LoadNffFile.cpp
    LoadObjFile.cpp
    main.cpp
    RatGeometry.cpp
    RayTraceSetup.cpp
    RayTraceStats.cpp
    SceneDescription.cpp
)

target_link_libraries(Gray Physics)
target_link_libraries(Gray Sources)
target_link_libraries(Gray Output)
target_link_libraries(Gray DataStructs)
target_link_libraries(Gray VrMath)
target_link_libraries(Gray Graphics)
target_link_libraries(Gray OpenGLRender)

find_package(OpenGL REQUIRED)
target_link_libraries(Gray ${OPENGL_gl_LIBRARIES})
find_package(GLUT REQUIRED)
target_link_libraries(Gray ${GLUT_LIBRARIES})
