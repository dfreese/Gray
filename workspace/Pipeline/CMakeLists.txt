add_library(Pipeline
    include/Pipeline/blur.h
    src/Pipeline.cpp
    include/Pipeline/blurprocess.h
    include/Pipeline/coincprocess.h
    include/Pipeline/filterprocess.h
    include/Pipeline/mergeprocess.h
    include/Pipeline/processor.h
    include/Pipeline/processstream.h
    include/Pipeline/singlesstream.h
    include/Pipeline/sortprocess.h
)

target_include_directories(Pipeline PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

