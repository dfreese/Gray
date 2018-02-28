add_executable(gray-daq
    daqmodel.cpp
)
target_link_libraries(gray-daq gammaray)
if (STATIC_BIN)
    target_link_libraries(gray-daq "-static")
endif (STATIC_BIN)

add_custom_command(TARGET gray-daq POST_BUILD 
    COMMAND "${CMAKE_COMMAND}" -E copy  "$<TARGET_FILE:gray-daq>" "${CMAKE_SOURCE_DIR}/../bin/")
