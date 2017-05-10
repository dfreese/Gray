cmake_minimum_required (VERSION 2.8.12)
project (Gray)

option(USE_OPENGL "Declares whether or not OpenGL should be used.  If not then
then the simulation viewer is not built." OFF)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

# Adapted from http://stackoverflow.com/a/7750816/2465202
set(BIN_DIR "../bin")
set(LIB_DIR "../lib")
# First for the generic no-config case
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR})
# Second, for multi-config builds (i.e. Debug, Release, RelWithDebInfo)
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BIN_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${LIB_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${LIB_DIR})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

add_library(Gray
    Graphics/src/CameraView.cpp
    Graphics/include/Graphics/CameraView.h
    Graphics/src/DirectLight.cpp
    Graphics/include/Graphics/DirectLight.h
    Graphics/src/Extents.cpp
    Graphics/include/Graphics/Extents.h
    Graphics/include/Graphics/Light.h
    Graphics/src/Material.cpp
    Graphics/include/Graphics/Material.h
    Graphics/include/Graphics/MaterialBase.h
    Graphics/src/SceneDescription.cpp
    Graphics/include/Graphics/SceneDescription.h
    Graphics/src/TransformViewable.cpp
    Graphics/include/Graphics/TransformViewable.h
    Graphics/include/Graphics/View.h
    Graphics/src/ViewableBase.cpp
    Graphics/include/Graphics/ViewableBase.h
    Graphics/src/ViewableBezierSet.cpp
    Graphics/include/Graphics/ViewableBezierSet.h
    Graphics/src/ViewableCone.cpp
    Graphics/include/Graphics/ViewableCone.h
    Graphics/src/ViewableCylinder.cpp
    Graphics/include/Graphics/ViewableCylinder.h
    Graphics/src/ViewableEllipsoid.cpp
    Graphics/include/Graphics/ViewableEllipsoid.h
    Graphics/src/ViewableParallelepiped.cpp
    Graphics/include/Graphics/ViewableParallelepiped.h
    Graphics/src/ViewableParallelogram.cpp
    Graphics/include/Graphics/ViewableParallelogram.h
    Graphics/src/ViewableSphere.cpp
    Graphics/include/Graphics/ViewableSphere.h
    Graphics/src/ViewableTorus.cpp
    Graphics/include/Graphics/ViewableTorus.h
    Graphics/src/ViewableTriangle.cpp
    Graphics/include/Graphics/ViewableTriangle.h
    Graphics/include/Graphics/VisiblePoint.h
    GraphicsTrees/src/IntersectionKdTree.cpp
    GraphicsTrees/include/GraphicsTrees/IntersectionKdTree.h
    Gray/include/Gray/Config.h
    Gray/src/Config.cpp
    Gray/include/Gray/GammaMaterial.h
    Gray/src/GammaRayTrace.cpp
    Gray/include/Gray/GammaRayTrace.h
    Gray/src/LoadDetector.cpp
    Gray/include/Gray/LoadDetector.h
    Gray/src/LoadMaterials.cpp
    Gray/include/Gray/LoadMaterials.h
    KdTree/src/DoubleRecurse.cpp
    KdTree/include/KdTree/DoubleRecurse.h
    KdTree/include/KdTree/KdTree.h
    KdTree/src/KdTree.cpp
    Output/include/Output/BinaryFormat.h
    Output/src/Detector.cpp
    Output/include/Output/Detector.h
    Output/src/DetectorArray.cpp
    Output/include/Output/DetectorArray.h
    Output/src/Output.cpp
    Output/include/Output/Output.h
    Physics/src/Beam.cpp
    Physics/include/Physics/Beam.h
    Physics/src/GammaDecay.cpp
    Physics/include/Physics/GammaDecay.h
    Physics/src/GammaStats.cpp
    Physics/include/Physics/GammaStats.h
    Physics/src/Interaction.cpp
    Physics/include/Physics/Interaction.h
    Physics/src/Isotope.cpp
    Physics/include/Physics/Isotope.h
    Physics/src/NuclearDecay.cpp
    Physics/include/Physics/NuclearDecay.h
    Physics/src/BeamDecay.cpp
    Physics/include/Physics/BeamDecay.h
    Physics/src/Photon.cpp
    Physics/include/Physics/Photon.h
    Physics/src/Positron.cpp
    Physics/include/Physics/Positron.h
    Physics/src/PositronDecay.cpp
    Physics/include/Physics/PositronDecay.h
    Pipeline/include/Pipeline/blur.h
    Pipeline/include/Pipeline/blurprocess.h
    Pipeline/include/Pipeline/coincprocess.h
    Pipeline/include/Pipeline/deadtimeprocess.h
    Pipeline/include/Pipeline/filterprocess.h
    Pipeline/include/Pipeline/mergeprocess.h
    Pipeline/include/Pipeline/processor.h
    Pipeline/include/Pipeline/processstream.h
    Pipeline/include/Pipeline/singlesstream.h
    Pipeline/include/Pipeline/sortprocess.h
    Random/src/Random.cpp
    Random/include/Random/Random.h
    Sources/src/AnnulusCylinderSource.cpp
    Sources/include/Sources/AnnulusCylinderSource.h
    Sources/src/AnnulusEllipticCylinderSource.cpp
    Sources/include/Sources/AnnulusEllipticCylinderSource.h
    Sources/src/BeamPointSource.cpp
    Sources/include/Sources/BeamPointSource.h
    Sources/src/CylinderSource.cpp
    Sources/include/Sources/CylinderSource.h
    Sources/src/EllipsoidSource.cpp
    Sources/include/Sources/EllipsoidSource.h
    Sources/src/EllipticCylinderSource.cpp
    Sources/include/Sources/EllipticCylinderSource.h
    Sources/src/RectSource.cpp
    Sources/include/Sources/RectSource.h
    Sources/include/Sources/Source.h
    Sources/src/SourceList.cpp
    Sources/include/Sources/SourceList.h
    Sources/src/SphereSource.cpp
    Sources/include/Sources/SphereSource.h
    Sources/src/VectorSource.cpp
    Sources/include/Sources/VectorSource.h
    Sources/src/VoxelSource.cpp
    Sources/include/Sources/VoxelSource.h
    VrMath/src/Aabb.cpp
    VrMath/include/VrMath/Aabb.h
    VrMath/src/LinearR2.cpp
    VrMath/include/VrMath/LinearR2.h
    VrMath/src/LinearR3.cpp
    VrMath/include/VrMath/LinearR3.h
    VrMath/src/LinearR4.cpp
    VrMath/include/VrMath/LinearR4.h
    VrMath/include/VrMath/MathMisc.h
    VrMath/src/Parallelepiped.cpp
    VrMath/include/VrMath/Parallelepiped.h
    VrMath/src/PolygonClip.cpp
    VrMath/include/VrMath/PolygonClip.h
    VrMath/src/PolynomialRC.cpp
    VrMath/include/VrMath/PolynomialRC.h
    VrMath/src/Quaternion.cpp
    VrMath/include/VrMath/Quaternion.h
    VrMath/include/VrMath/VrData.h
)
target_include_directories(Gray PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/include
    ${CMAKE_CURRENT_SOURCE_DIR}/GraphicsTrees/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Gray/include
    ${CMAKE_CURRENT_SOURCE_DIR}/KdTree/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Output/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Physics/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Pipeline/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Random/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Sources/include
    ${CMAKE_CURRENT_SOURCE_DIR}/VrMath/include
)

add_executable(gray
    Gray/src/main.cpp
)
target_link_libraries(gray Gray)

add_executable(gray-daq
    daqmodel/src/daqmodel.cpp
)
target_link_libraries(gray-daq Gray)
