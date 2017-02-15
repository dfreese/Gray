add_library(Output
    include/Output/BinaryFormat.h
    src/Detector.cpp
    include/Output/Detector.h
    src/DetectorArray.cpp
    include/Output/DetectorArray.h
    src/Output.cpp
    include/Output/Output.h
)

target_include_directories(Output PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Output Random)
target_link_libraries(Output Graphics)
target_link_libraries(Output Physics)
target_link_libraries(Output VrMath)
