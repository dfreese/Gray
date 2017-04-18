add_library(Physics
    src/Beam.cpp
    include/Physics/Beam.h
    src/GammaDecay.cpp
    include/Physics/GammaDecay.h
    src/GammaStats.cpp
    include/Physics/GammaStats.h
    src/Interaction.cpp
    include/Physics/Interaction.h
    src/Isotope.cpp
    include/Physics/Isotope.h
    src/NuclearDecay.cpp
    include/Physics/NuclearDecay.h
    src/BeamDecay.cpp
    include/Physics/BeamDecay.h
    src/Photon.cpp
    include/Physics/Photon.h
    src/Positron.cpp
    include/Physics/Positron.h
    src/PositronDecay.cpp
    include/Physics/PositronDecay.h
)

target_include_directories(Physics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Physics Random)
target_link_libraries(Physics VrMath)
