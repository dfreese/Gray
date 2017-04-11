cmake_minimum_required (VERSION 2.8.12)
project (Gray)

option(USE_OPENGL "Declares whether or not OpenGL should be used.  If not then
then the simulation viewer is not built." ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(KdTree)
add_subdirectory(Graphics)
add_subdirectory(GraphicsTrees)
add_subdirectory(Random)
add_subdirectory(VrMath)
add_subdirectory(Physics)
add_subdirectory(Sources)
add_subdirectory(Output)
add_subdirectory(Pipeline)
if (USE_OPENGL)
    add_subdirectory(Viewer)
endif (USE_OPENGL)
add_subdirectory(Gray)
