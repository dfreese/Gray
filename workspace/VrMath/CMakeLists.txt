add_library(VrMath
    Aabb.cpp
    Aabb.h
    LinearR2.cpp
    LinearR2.h
    LinearR3.cpp
    LinearR3.h
    LinearR4.cpp
    LinearR4.h
    MathMisc.h
    Parallelepiped.cpp
    Parallelepiped.h
    PolygonClip.cpp
    PolygonClip.h
    PolynomialRC.cpp
    PolynomialRC.h
    Quaternion.cpp
    Quaternion.h
    VrData.h
)

target_include_directories(VrMath PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
