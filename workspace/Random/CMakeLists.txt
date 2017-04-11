add_library(Random
    src/Random.cpp
    include/Random/Random.h
)

target_include_directories(Random PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Random VrMath)
