add_library(Gray
    3ds.cpp
    GammaRayTrace.cpp
    RatGeometry.cpp
)

target_include_directories(Gray PUBLIC $(CMAKE_CURRENT_SOURCE_DIR))

target_link_libraries(Gray Physics)
target_link_libraries(Gray Sources)
target_link_libraries(Gray Output)
target_link_libraries(Gray DataStructs)
target_link_libraries(Gray VrMath)
target_link_libraries(Gray Graphics)
