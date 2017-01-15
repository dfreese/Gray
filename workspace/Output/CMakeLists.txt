add_library(Output
    Coincidence.cpp
    Detector.cpp
    DetectorArray.cpp
    Event.cpp
    EventBuffer.cpp
    Output.cpp
)

target_include_directories(Output PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Output Random)
target_link_libraries(Output Graphics)
target_link_libraries(Output Physics)
target_link_libraries(Output DataStructs)
