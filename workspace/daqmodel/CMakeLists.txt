add_executable(daqmodel
    src/daqmodel.cpp
)

target_include_directories(daqmodel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(daqmodel Output)
target_link_libraries(daqmodel Pipeline)
