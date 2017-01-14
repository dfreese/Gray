add_library(Random
    mt19937.cpp
    Random.cpp
)

target_include_directories(Random PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Random VrMath)
