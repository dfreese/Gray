/*
 *
 * RayTrace Software Package, release 3.0.  May 3, 2006
 *
 * Author: Samuel R. Buss
 *
 * Software accompanying the book
 *		3D Computer Graphics: A Mathematical Introduction with OpenGL,
 *		by S. Buss, Cambridge University Press, 2003.
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.  Please acknowledge
 *   all use of the software in any publications or products based on it.
 *
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG
 *
 */

#include <Graphics/SceneDescription.h>


SceneDescription::SceneDescription()
{
    TheBackgroundColor.Set( 0.0, 0.0, 0.0 );
    TheGlobalAmbientLight.SetZero();
    ScreenRegistered = false;
}

int SceneDescription::AddLight( Light* newLight )
{
    int index = (int)LightArray.size();
    LightArray.push_back(newLight);
    return index;
}

int SceneDescription::AddMaterial(MaterialBase * newMaterial )
{
    int index = (int)MaterialArray.size();
    MaterialArray.push_back(newMaterial);
    material_names_map[newMaterial->GetName()] = index;
    return index;
}

int SceneDescription::AddViewable( ViewableBase* newViewable )
{
    int index = (int)ViewableArray.size();
    ViewableArray.push_back(newViewable);
    return index;
}

void SceneDescription::DeleteAll()
{
    DeleteAllLights();
    DeleteAllMaterials();
    DeleteAllViewables();
}

SceneDescription::~SceneDescription()
{
    DeleteAll();
}

// Once you have set up an initial CameraView, you can call RegisterCameraView.
//  After that, you may call CalcNewSceenDims( aspectRatio ) to get
//	a suggested width and height for the camera screen.
void SceneDescription::RegisterCameraView()
{
    RegisteredScreenWidth = CameraAndViewer.GetScreenWidth();
    RegisteredScreenHeight = CameraAndViewer.GetScreenHeight();
    ScreenRegistered = true;
}

void SceneDescription::CalcNewScreenDims( float aspectRatio )
{
    assert ( ScreenRegistered );
    assert ( aspectRatio>0.0 );
    if ( ScreenRegistered ) {
        double registeredAspectRatio = RegisteredScreenWidth/RegisteredScreenHeight;
        if ( registeredAspectRatio <= aspectRatio ) {
            // Match up heights
            CameraAndViewer.SetScreenDimensions( RegisteredScreenHeight*aspectRatio, RegisteredScreenHeight );
        } else {
            // Match up widths
            CameraAndViewer.SetScreenDimensions( RegisteredScreenWidth, RegisteredScreenWidth/aspectRatio );
        }
    }
}

void SceneDescription::DeleteAllLights()
{
    while (!LightArray.empty()) {
        delete LightArray.back();
        LightArray.pop_back();
    }
}

void SceneDescription::DeleteAllMaterials()
{
    while (!MaterialArray.empty()) {
        delete MaterialArray.back();
        MaterialArray.pop_back();
    }
}

void SceneDescription::DeleteAllViewables()
{
    while (!ViewableArray.empty()) {
        delete ViewableArray.back();
        ViewableArray.pop_back();
    }
}

AABB SceneDescription::GetExtents() {
    AABB scene_aabb;
    for (size_t idx = 0; idx < NumViewables(); idx++) {
        AABB aabb;
        GetViewable(idx).CalcAABB(aabb);
        scene_aabb.EnlargeToEnclose(aabb);
    }
    return(scene_aabb);
}

double SceneDescription::GetMaxDistance() {
    AABB extents = GetExtents();
    return((extents.GetBoxMax() - extents.GetBoxMin()).Norm());
}