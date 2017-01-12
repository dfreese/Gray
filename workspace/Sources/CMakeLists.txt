add_library(Sources
    AnnulusCylinderSource.cpp
    AnnulusEllipticCylinderSource.cpp
    BeamPointSource.cpp
    CylinderSource.cpp
    EllipsoidSource.cpp
    EllipticCylinderSource.cpp
    RectSource.cpp
    SourceList.cpp
    SphereSource.cpp
    VectorSource.cpp
    VoxelSource.cpp
)

target_include_directories(Sources PUBLIC $(CMAKE_CURRENT_SOURCE_DIR))

target_link_libraries(Sources Graphics)
target_link_libraries(Sources VrMath)
target_link_libraries(Sources Physics)
target_link_libraries(Sources Random)
