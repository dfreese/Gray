include(GetGitRevisionDescription)
git_describe(GIT_SHA1 --match=NeVeRmAtCh --always --abbrev=10)
string(TIMESTAMP CURRENT_YEAR "%Y")
configure_file(Version/Version.h.in ${CMAKE_BINARY_DIR}/generated/Gray/Version/Version.h)

add_library(gammaray
    Daq/BlurProcess.cpp
    Daq/BlurFunctors.cpp
    Daq/CoincProcess.cpp
    Daq/DaqModel.cpp
    Daq/DeadtimeProcess.cpp
    Daq/FilterProcess.cpp
    Daq/FilterFunctors.cpp
    Daq/Mapping.cpp
    Daq/MergeProcess.cpp
    Daq/MergeFunctors.cpp
    Daq/SortProcess.cpp
    Daq/Process.cpp
    Daq/ProcessFactory.cpp
    Graphics/CameraView.cpp
    Graphics/Material.cpp
    Graphics/SceneDescription.cpp
    Graphics/TransformViewable.cpp
    Graphics/ViewableBase.cpp
    Graphics/ViewableBezierSet.cpp
    Graphics/ViewableCone.cpp
    Graphics/ViewableCylinder.cpp
    Graphics/ViewableEllipsoid.cpp
    Graphics/ViewableParallelepiped.cpp
    Graphics/ViewableParallelogram.cpp
    Graphics/ViewableSphere.cpp
    Graphics/ViewableTorus.cpp
    Graphics/ViewableTriangle.cpp
    Gray/Command.cpp
    Gray/Config.cpp
    Gray/File.cpp
    Gray/GammaMaterial.cpp
    Gray/GammaRayTrace.cpp
    Gray/Load.cpp
    Gray/LoadMaterials.cpp
    Gray/Simulation.cpp
    Gray/Syntax.cpp
    KdTree/DoubleRecurse.cpp
    KdTree/KdTree.cpp
    Math/Math.cpp
    Output/Detector.cpp
    Output/DetectorArray.cpp
    Output/Input.cpp
    Output/IO.cpp
    Output/Output.cpp
    Physics/Beam.cpp
    Physics/Compton.cpp
    Physics/GammaStats.cpp
    Physics/GaussianBeam.cpp
    Physics/Interaction.cpp
    Physics/Isotope.cpp
    Physics/KleinNishina.cpp
    Physics/NuclearDecay.cpp
    Physics/Photon.cpp
    Physics/Positron.cpp
    Physics/Physics.cpp
    Physics/Rayleigh.cpp
    Physics/Thompson.cpp
    Random/Random.cpp
    Random/Transform.cpp
    Sources/AnnulusCylinderSource.cpp
    Sources/AnnulusEllipticCylinderSource.cpp
    Sources/CylinderSource.cpp
    Sources/EllipsoidSource.cpp
    Sources/EllipticCylinderSource.cpp
    Sources/PointSource.cpp
    Sources/RectSource.cpp
    Sources/SourceList.cpp
    Sources/SphereSource.cpp
    Sources/VectorSource.cpp
    Sources/VoxelSource.cpp
    VrMath/Aabb.cpp
    VrMath/LinearR2.cpp
    VrMath/LinearR3.cpp
    VrMath/LinearR4.cpp
    VrMath/PolygonClip.cpp
    VrMath/PolynomialRC.cpp
    VrMath/Quaternion.cpp
    json/jsoncpp.cpp
    )
target_include_directories(gammaray PUBLIC ../include ${CMAKE_BINARY_DIR}/generated)
target_compile_options(gammaray PRIVATE -Wall -Wextra -Werror)


################################################################################
find_package(Threads REQUIRED)

add_executable(gray
    Gray/gray.cpp
    )
target_link_libraries(gray PUBLIC gammaray ${CMAKE_THREAD_LIBS_INIT})
target_compile_options(gray PRIVATE -Wall -Wextra -Werror)
if (STATIC_BIN)
    target_compile_options(gray PRIVATE -static)
endif (STATIC_BIN)
add_custom_command(TARGET gray POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy  "$<TARGET_FILE:gray>" "${CMAKE_SOURCE_DIR}/bin/")


################################################################################
add_executable(gray-daq
    Gray/gray-daq.cpp
)
target_link_libraries(gray-daq PUBLIC gammaray)
target_compile_options(gray-daq PRIVATE -Wall -Wextra -Werror)
if (STATIC_BIN)
    target_compile_options(gray-daq PRIVATE -static)
endif (STATIC_BIN)

add_custom_command(TARGET gray-daq POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy  "$<TARGET_FILE:gray-daq>" "${CMAKE_SOURCE_DIR}/bin/")


################################################################################
find_package(OpenGL)
find_package(GLUT)

if (OPENGL_FOUND AND OPENGL_GLU_FOUND AND GLUT_FOUND)
    add_executable(gray-view
        Viewer/gray-view.cpp
        Viewer/GlutRenderer.cpp
        Viewer/Viewer.cpp
        )
    target_include_directories(gray-view PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(gray-view PUBLIC
        gammaray ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
    target_compile_options(gray-view PRIVATE -Wall -Wextra -Werror)

    add_custom_command(TARGET gray-view POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy  "$<TARGET_FILE:gray-view>" "${CMAKE_SOURCE_DIR}/bin/")
endif (OPENGL_FOUND AND OPENGL_GLU_FOUND AND GLUT_FOUND)
