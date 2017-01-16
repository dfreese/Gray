add_library(Random
    mt19937.cpp
    mt19937.h
    Random.cpp
    Random.h
)

target_include_directories(Random PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Random VrMath)
