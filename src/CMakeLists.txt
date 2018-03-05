include(GetGitRevisionDescription)
git_describe(GIT_SHA1 --match=NeVeRmAtCh --always --abbrev=10)
string(TIMESTAMP CURRENT_YEAR "%Y")
configure_file(Version/Version.h.in ${CMAKE_BINARY_DIR}/generated/Gray/Version/Version.h)

add_library(gammaray
    ${CMAKE_SOURCE_DIR}/include/Gray/Daq/BlurProcess.h
    ${CMAKE_SOURCE_DIR}/src/Daq/BlurProcess.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Daq/BlurFunctors.h
    ${CMAKE_SOURCE_DIR}/src/Daq/BlurFunctors.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Daq/CoincProcess.h
    ${CMAKE_SOURCE_DIR}/src/Daq/CoincProcess.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Daq/DaqModel.h
    ${CMAKE_SOURCE_DIR}/src/Daq/DaqModel.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Daq/DaqStats.h
    ${CMAKE_SOURCE_DIR}/include/Gray/Daq/DeadtimeProcess.h
    ${CMAKE_SOURCE_DIR}/src/Daq/DeadtimeProcess.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Daq/FilterProcess.h
    ${CMAKE_SOURCE_DIR}/src/Daq/FilterProcess.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Daq/FilterFunctors.h
    ${CMAKE_SOURCE_DIR}/src/Daq/FilterFunctors.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Daq/Mapping.h
    ${CMAKE_SOURCE_DIR}/src/Daq/Mapping.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Daq/MergeProcess.h
    ${CMAKE_SOURCE_DIR}/src/Daq/MergeProcess.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Daq/MergeFunctors.h
    ${CMAKE_SOURCE_DIR}/src/Daq/MergeFunctors.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Daq/SortProcess.h
    ${CMAKE_SOURCE_DIR}/src/Daq/SortProcess.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Daq/Process.h
    ${CMAKE_SOURCE_DIR}/src/Daq/Process.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Daq/ProcessFactory.h
    ${CMAKE_SOURCE_DIR}/src/Daq/ProcessFactory.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Daq/ProcessStats.h
    ${CMAKE_SOURCE_DIR}/src/Graphics/CameraView.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Graphics/CameraView.h
    ${CMAKE_SOURCE_DIR}/include/Gray/Graphics/Light.h
    ${CMAKE_SOURCE_DIR}/include/Gray/Graphics/Material.h
    ${CMAKE_SOURCE_DIR}/src/Graphics/Material.cpp
    ${CMAKE_SOURCE_DIR}/src/Graphics/SceneDescription.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Graphics/SceneDescription.h
    ${CMAKE_SOURCE_DIR}/src/Graphics/TransformViewable.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Graphics/TransformViewable.h
    ${CMAKE_SOURCE_DIR}/include/Gray/Graphics/View.h
    ${CMAKE_SOURCE_DIR}/src/Graphics/ViewableBase.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Graphics/ViewableBase.h
    ${CMAKE_SOURCE_DIR}/src/Graphics/ViewableBezierSet.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Graphics/ViewableBezierSet.h
    ${CMAKE_SOURCE_DIR}/src/Graphics/ViewableCone.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Graphics/ViewableCone.h
    ${CMAKE_SOURCE_DIR}/src/Graphics/ViewableCylinder.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Graphics/ViewableCylinder.h
    ${CMAKE_SOURCE_DIR}/src/Graphics/ViewableEllipsoid.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Graphics/ViewableEllipsoid.h
    ${CMAKE_SOURCE_DIR}/src/Graphics/ViewableParallelepiped.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Graphics/ViewableParallelepiped.h
    ${CMAKE_SOURCE_DIR}/src/Graphics/ViewableParallelogram.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Graphics/ViewableParallelogram.h
    ${CMAKE_SOURCE_DIR}/src/Graphics/ViewableSphere.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Graphics/ViewableSphere.h
    ${CMAKE_SOURCE_DIR}/src/Graphics/ViewableTorus.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Graphics/ViewableTorus.h
    ${CMAKE_SOURCE_DIR}/src/Graphics/ViewableTriangle.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Graphics/ViewableTriangle.h
    ${CMAKE_SOURCE_DIR}/include/Gray/Graphics/VisiblePoint.h
    ${CMAKE_SOURCE_DIR}/include/Gray/Gray/Config.h
    ${CMAKE_SOURCE_DIR}/src/Gray/Config.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Gray/GammaMaterial.h
    ${CMAKE_SOURCE_DIR}/src/Gray/GammaMaterial.cpp
    ${CMAKE_SOURCE_DIR}/src/Gray/GammaRayTrace.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Gray/GammaRayTrace.h
    ${CMAKE_SOURCE_DIR}/include/Gray/Gray/GammaRayTraceStats.h
    ${CMAKE_SOURCE_DIR}/src/Gray/LoadDetector.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Gray/LoadDetector.h
    ${CMAKE_SOURCE_DIR}/src/Gray/LoadMaterials.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Gray/LoadMaterials.h
    ${CMAKE_SOURCE_DIR}/src/Gray/Simulation.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Gray/Simulation.h
    ${CMAKE_SOURCE_DIR}/include/Gray/Gray/SimulationStats.h
    ${CMAKE_SOURCE_DIR}/include/Gray/Gray/Syntax.h
    ${CMAKE_SOURCE_DIR}/src/Gray/Syntax.cpp
    ${CMAKE_SOURCE_DIR}/src/KdTree/DoubleRecurse.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/KdTree/DoubleRecurse.h
    ${CMAKE_SOURCE_DIR}/include/Gray/KdTree/KdTree.h
    ${CMAKE_SOURCE_DIR}/src/KdTree/KdTree.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Math/Math.h
    ${CMAKE_SOURCE_DIR}/src/Math/Math.cpp
    ${CMAKE_SOURCE_DIR}/src/Output/Detector.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Output/Detector.h
    ${CMAKE_SOURCE_DIR}/src/Output/DetectorArray.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Output/DetectorArray.h
    ${CMAKE_SOURCE_DIR}/src/Output/Input.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Output/Input.h
    ${CMAKE_SOURCE_DIR}/src/Output/IO.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Output/IO.h
    ${CMAKE_SOURCE_DIR}/src/Output/Output.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Output/Output.h
    ${CMAKE_SOURCE_DIR}/src/Physics/Beam.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Physics/Beam.h
    ${CMAKE_SOURCE_DIR}/src/Physics/Compton.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Physics/Compton.h
    ${CMAKE_SOURCE_DIR}/src/Physics/GammaStats.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Physics/GammaStats.h
    ${CMAKE_SOURCE_DIR}/src/Physics/Interaction.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Physics/Interaction.h
    ${CMAKE_SOURCE_DIR}/src/Physics/Isotope.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Physics/Isotope.h
    ${CMAKE_SOURCE_DIR}/src/Physics/KleinNishina.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Physics/KleinNishina.h
    ${CMAKE_SOURCE_DIR}/src/Physics/NuclearDecay.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Physics/NuclearDecay.h
    ${CMAKE_SOURCE_DIR}/src/Physics/Photon.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Physics/Photon.h
    ${CMAKE_SOURCE_DIR}/src/Physics/Positron.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Physics/Positron.h
    ${CMAKE_SOURCE_DIR}/src/Physics/Physics.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Physics/Physics.h
    ${CMAKE_SOURCE_DIR}/src/Physics/Rayleigh.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Physics/Rayleigh.h
    ${CMAKE_SOURCE_DIR}/src/Physics/Thompson.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Physics/Thompson.h
    ${CMAKE_SOURCE_DIR}/src/Random/Random.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Random/Random.h
    ${CMAKE_SOURCE_DIR}/include/Gray/Random/Transform.h
    ${CMAKE_SOURCE_DIR}/src/Random/Transform.cpp
    ${CMAKE_SOURCE_DIR}/src/Sources/AnnulusCylinderSource.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Sources/AnnulusCylinderSource.h
    ${CMAKE_SOURCE_DIR}/src/Sources/AnnulusEllipticCylinderSource.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Sources/AnnulusEllipticCylinderSource.h
    ${CMAKE_SOURCE_DIR}/src/Sources/CylinderSource.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Sources/CylinderSource.h
    ${CMAKE_SOURCE_DIR}/src/Sources/EllipsoidSource.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Sources/EllipsoidSource.h
    ${CMAKE_SOURCE_DIR}/src/Sources/EllipticCylinderSource.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Sources/EllipticCylinderSource.h
    ${CMAKE_SOURCE_DIR}/src/Sources/PointSource.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Sources/PointSource.h
    ${CMAKE_SOURCE_DIR}/src/Sources/RectSource.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Sources/RectSource.h
    ${CMAKE_SOURCE_DIR}/include/Gray/Sources/Source.h
    ${CMAKE_SOURCE_DIR}/src/Sources/SourceList.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Sources/SourceList.h
    ${CMAKE_SOURCE_DIR}/src/Sources/SphereSource.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Sources/SphereSource.h
    ${CMAKE_SOURCE_DIR}/src/Sources/VectorSource.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Sources/VectorSource.h
    ${CMAKE_SOURCE_DIR}/src/Sources/VoxelSource.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/Sources/VoxelSource.h
    ${CMAKE_SOURCE_DIR}/src/VrMath/Aabb.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/VrMath/Aabb.h
    ${CMAKE_SOURCE_DIR}/src/VrMath/LinearR2.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/VrMath/LinearR2.h
    ${CMAKE_SOURCE_DIR}/src/VrMath/LinearR3.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/VrMath/LinearR3.h
    ${CMAKE_SOURCE_DIR}/src/VrMath/LinearR4.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/VrMath/LinearR4.h
    ${CMAKE_SOURCE_DIR}/include/Gray/VrMath/MathMisc.h
    ${CMAKE_SOURCE_DIR}/src/VrMath/PolygonClip.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/VrMath/PolygonClip.h
    ${CMAKE_SOURCE_DIR}/src/VrMath/PolynomialRC.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/VrMath/PolynomialRC.h
    ${CMAKE_SOURCE_DIR}/src/VrMath/Quaternion.cpp
    ${CMAKE_SOURCE_DIR}/include/Gray/VrMath/Quaternion.h
    ${CMAKE_BINARY_DIR}/generated/Gray/Version/Version.h
    ${CMAKE_SOURCE_DIR}/src/json/jsoncpp.cpp
)
target_include_directories(gammaray PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/generated)


################################################################################
find_package(Threads REQUIRED)

add_executable(gray
    ${CMAKE_SOURCE_DIR}/src/Gray/gray.cpp
    )
target_link_libraries(gray gammaray ${CMAKE_THREAD_LIBS_INIT})
if (STATIC_BIN)
    target_link_libraries(gray "-static")
endif (STATIC_BIN)
add_custom_command(TARGET gray POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy  "$<TARGET_FILE:gray>" "${CMAKE_SOURCE_DIR}/bin/")


################################################################################
add_executable(gray-daq
    ${CMAKE_SOURCE_DIR}/src/Gray/gray-daq.cpp
)
target_link_libraries(gray-daq gammaray)
if (STATIC_BIN)
    target_link_libraries(gray-daq "-static")
endif (STATIC_BIN)

add_custom_command(TARGET gray-daq POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy  "$<TARGET_FILE:gray-daq>" "${CMAKE_SOURCE_DIR}/bin/")


################################################################################
find_package(OpenGL)
find_package(GLUT)

if (OPENGL_FOUND AND OPENGL_GLU_FOUND AND GLUT_FOUND)
    add_executable(gray-view
        ${CMAKE_SOURCE_DIR}/src/Viewer/gray-view.cpp
        ${CMAKE_SOURCE_DIR}/src/Viewer/GlutRenderer.cpp
        ${CMAKE_SOURCE_DIR}/include/Gray/Viewer/GlutRenderer.h
        ${CMAKE_SOURCE_DIR}/src/Viewer/Viewer.cpp
        ${CMAKE_SOURCE_DIR}/include/Gray/Viewer/Viewer.h
        )
    target_include_directories(gray-view PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(gray-view ${OPENGL_LIBRARIES})
    target_link_libraries(gray-view ${GLUT_LIBRARIES})
    target_link_libraries(gray-view gammaray)

    add_custom_command(TARGET gray-view POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy  "$<TARGET_FILE:gray-view>" "${CMAKE_SOURCE_DIR}/bin/")
endif (OPENGL_FOUND AND OPENGL_GLU_FOUND AND GLUT_FOUND)
