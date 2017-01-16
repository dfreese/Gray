add_library(Output
    BinaryFormat.h
    Coincidence.cpp
    Coincidence.h
    Detector.cpp
    Detector.h
    DetectorArray.cpp
    DetectorArray.h
    Event.cpp
    Event.h
    EventBuffer.cpp
    EventBuffer.h
    Output.cpp
    Output.h
)

target_include_directories(Output PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Output Random)
target_link_libraries(Output Graphics)
target_link_libraries(Output Physics)
target_link_libraries(Output DataStructs)
