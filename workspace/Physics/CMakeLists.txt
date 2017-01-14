add_library(Physics
    BackBack.cpp
    Beam.cpp
    Compton.cpp
    Deposit.cpp
    F18.cpp
    GammaDecay.cpp
    GammaStats.cpp
    IN110.cpp
    Interaction.cpp
    InteractionList.cpp
    Isotope.cpp
    NuclearDecay.cpp
    Photon.cpp
    Positron.cpp
    PositronDecay.cpp
    ZR89.cpp
)

target_include_directories(Physics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Physics VrMath)
target_link_libraries(Physics Random)
target_link_libraries(Physics Output)
