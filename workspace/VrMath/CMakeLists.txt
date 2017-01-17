add_library(VrMath
    src/Aabb.cpp
    include/VrMath/Aabb.h
    src/LinearR2.cpp
    include/VrMath/LinearR2.h
    src/LinearR3.cpp
    include/VrMath/LinearR3.h
    src/LinearR4.cpp
    include/VrMath/LinearR4.h
    include/VrMath/MathMisc.h
    src/Parallelepiped.cpp
    include/VrMath/Parallelepiped.h
    src/PolygonClip.cpp
    include/VrMath/PolygonClip.h
    src/PolynomialRC.cpp
    include/VrMath/PolynomialRC.h
    src/Quaternion.cpp
    include/VrMath/Quaternion.h
    include/VrMath/VrData.h
)

target_include_directories(VrMath PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
