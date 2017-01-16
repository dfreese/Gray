add_library(Physics
    BackBack.cpp
    BackBack.h
    Beam.cpp
    Beam.h
    Compton.cpp
    Compton.h
    Deposit.cpp
    Deposit.h
    F18.cpp
    F18.h
    GammaDecay.cpp
    GammaDecay.h
    GammaMaterial.h
    GammaStats.cpp
    GammaStats.h
    IN110.cpp
    IN110.h
    Interaction.cpp
    Interaction.h
    InteractionList.cpp
    InteractionList.h
    Isotope.cpp
    Isotope.h
    NuclearDecay.cpp
    NuclearDecay.h
    Photon.cpp
    Photon.h
    Positron.cpp
    Positron.h
    PositronDecay.cpp
    PositronDecay.h
    ZR89.cpp
    ZR89.h
)

target_include_directories(Physics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Physics VrMath)
target_link_libraries(Physics Random)
target_link_libraries(Physics Output)
