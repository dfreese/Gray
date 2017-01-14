add_library(Graphics
    BumpMapFunction.cpp
    CameraView.cpp
    DirectLight.cpp
    Extents.cpp
    MaterialCookTorrance.cpp
    Material.cpp
    RgbImage.cpp
    TextureAffineXform.cpp
    TextureBilinearXform.cpp
    TextureCheckered.cpp
    TextureMapBase.cpp
    TextureMultiFaces.cpp
    TextureRgbImage.cpp
    TextureSequence.cpp
    TransformViewable.cpp
    ViewableBase.cpp
    ViewableBezierSet.cpp
    ViewableCone.cpp
    ViewableCylinder.cpp
    ViewableEllipsoid.cpp
    ViewableParallelepiped.cpp
    ViewableParallelogram.cpp
    ViewableSphere.cpp
    ViewableTorus.cpp
    ViewableTriangle.cpp
)

target_include_directories(Graphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(Graphics PUBLIC ${OPENGL_LIBRARIES})
