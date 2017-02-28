add_library(Random
    src/mt19937.cpp
    include/Random/mt19937.h
    src/Random.cpp
    include/Random/Random.h
)

target_include_directories(Random PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Random VrMath)
