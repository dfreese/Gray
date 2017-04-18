add_library(Physics
    src/BackBack.cpp
    include/Physics/BackBack.h
    src/Beam.cpp
    include/Physics/Beam.h
    src/F18.cpp
    include/Physics/F18.h
    src/GammaDecay.cpp
    include/Physics/GammaDecay.h
    src/GammaStats.cpp
    include/Physics/GammaStats.h
    src/IN110.cpp
    include/Physics/IN110.h
    src/Interaction.cpp
    include/Physics/Interaction.h
    src/Isotope.cpp
    include/Physics/Isotope.h
    src/NuclearDecay.cpp
    include/Physics/NuclearDecay.h
    src/O15.cpp
    include/Physics/O15.h
    src/BeamDecay.cpp
    include/Physics/BeamDecay.h
    src/Photon.cpp
    include/Physics/Photon.h
    src/Positron.cpp
    include/Physics/Positron.h
    src/PositronDecay.cpp
    include/Physics/PositronDecay.h
    src/ZR89.cpp
    include/Physics/ZR89.h
)

target_include_directories(Physics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Physics Random)
target_link_libraries(Physics VrMath)
