add_library(Sources
    AnnulusCylinderSource.cpp
    AnnulusCylinderSource.h
    AnnulusEllipticCylinderSource.cpp
    AnnulusEllipticCylinderSource.h
    BeamPointSource.cpp
    BeamPointSource.h
    CylinderSource.cpp
    CylinderSource.h
    EllipsoidSource.cpp
    EllipsoidSource.h
    EllipticCylinderSource.cpp
    EllipticCylinderSource.h
    RectSource.cpp
    RectSource.h
    Source.h
    SourceList.cpp
    SourceList.h
    SphereSource.cpp
    SphereSource.h
    VectorSource.cpp
    VectorSource.h
    VoxelSource.cpp
    VoxelSource.h
)

target_include_directories(Sources PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Sources Graphics)
target_link_libraries(Sources VrMath)
target_link_libraries(Sources Physics)
target_link_libraries(Sources Random)
