add_library(Graphics
    src/CameraView.cpp
    include/Graphics/CameraView.h
    src/DirectLight.cpp
    include/Graphics/DirectLight.h
    src/Extents.cpp
    include/Graphics/Extents.h
    include/Graphics/Light.h
    src/Material.cpp
    include/Graphics/Material.h
    include/Graphics/MaterialBase.h
    src/SceneDescription.cpp
    include/Graphics/SceneDescription.h
    src/TransformViewable.cpp
    include/Graphics/TransformViewable.h
    include/Graphics/View.h
    src/ViewableBase.cpp
    include/Graphics/ViewableBase.h
    src/ViewableBezierSet.cpp
    include/Graphics/ViewableBezierSet.h
    src/ViewableCone.cpp
    include/Graphics/ViewableCone.h
    src/ViewableCylinder.cpp
    include/Graphics/ViewableCylinder.h
    src/ViewableEllipsoid.cpp
    include/Graphics/ViewableEllipsoid.h
    src/ViewableParallelepiped.cpp
    include/Graphics/ViewableParallelepiped.h
    src/ViewableParallelogram.cpp
    include/Graphics/ViewableParallelogram.h
    src/ViewableSphere.cpp
    include/Graphics/ViewableSphere.h
    src/ViewableTorus.cpp
    include/Graphics/ViewableTorus.h
    src/ViewableTriangle.cpp
    include/Graphics/ViewableTriangle.h
    include/Graphics/VisiblePoint.h
)

target_include_directories(Graphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Graphics VrMath)
