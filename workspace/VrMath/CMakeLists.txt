add_library(VrMath
    Aabb.cpp
    LinearR2.cpp
    LinearR3.cpp
    LinearR4.cpp
    Parallelepiped.cpp
    PolygonClip.cpp
    PolynomialRC.cpp
    Quaternion.cpp
)

target_include_directories(VrMath PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
