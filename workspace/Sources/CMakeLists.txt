add_library(Sources
    src/AnnulusCylinderSource.cpp
    include/Sources/AnnulusCylinderSource.h
    src/AnnulusEllipticCylinderSource.cpp
    include/Sources/AnnulusEllipticCylinderSource.h
    src/BeamPointSource.cpp
    include/Sources/BeamPointSource.h
    src/CylinderSource.cpp
    include/Sources/CylinderSource.h
    src/EllipsoidSource.cpp
    include/Sources/EllipsoidSource.h
    src/EllipticCylinderSource.cpp
    include/Sources/EllipticCylinderSource.h
    src/RectSource.cpp
    include/Sources/RectSource.h
    include/Sources/Source.h
    src/SourceList.cpp
    include/Sources/SourceList.h
    src/SphereSource.cpp
    include/Sources/SphereSource.h
    src/VectorSource.cpp
    include/Sources/VectorSource.h
    src/VoxelSource.cpp
    include/Sources/VoxelSource.h
)

target_include_directories(Sources PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Sources Graphics)
target_link_libraries(Sources VrMath)
target_link_libraries(Sources Physics)
target_link_libraries(Sources Random)
target_link_libraries(Sources GraphicsTrees)
