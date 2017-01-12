cmake_minimum_required (VERSION 2.6)
project (Gray)

# http://stackoverflow.com/a/9017635/2465202
add_definitions(-DGAMMA_PHYSICS)

add_subdirectory(DataStructs)
add_subdirectory(Graphics)
add_subdirectory(Random)
add_subdirectory(VrMath)
add_subdirectory(Physics)
add_subdirectory(Sources)
add_subdirectory(Gray)
add_subdirectory(OpenGLRender)
add_subdirectory(RayTraceMgr)
add_subdirectory(RayTraceKd)
