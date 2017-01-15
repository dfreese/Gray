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

#include <SceneDescription.h>

// Garry: Added destructor for proper cleanup
SceneDescription::~SceneDescription()
{
    while (!LightArray.IsEmpty()) {
        Light * aLight = LightArray.Pop();
        delete aLight;
    }
    while (!MaterialArray.IsEmpty()) {
        GammaMaterial * material = MaterialArray.Pop();
        delete material;
    }
    while (!TextureArray.IsEmpty()) {
        TextureMapBase * texture = TextureArray.Pop();
        delete texture;
    }
    while (!ViewableArray.IsEmpty()) {
        ViewableBase * viewable = ViewableArray.Pop();
        delete viewable;
    }
    while (!GammaStatsArray.IsEmpty()) {
        // GammaStats are a property of GammaMaterial (not just a pointer)
        // These should not be deleted
        GammaStatsArray.Pop();
    }
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
    long i;
    for ( i=NumLights(); i>0; i-- ) {
        delete LightArray.Pop();
    }
}

void SceneDescription::DeleteAllMaterials()
{
    long i;
    for ( i=NumMaterials(); i>0; i-- ) {
        delete MaterialArray.Pop();
    }
}

void SceneDescription::DeleteAllTextures()
{
    long i;
    for ( i=NumTextures(); i>0; i-- ) {
        delete TextureArray.Pop();
    }
}

void SceneDescription::DeleteAllViewables()
{
    long i;
    for ( i=NumViewables(); i>0; i-- ) {
        delete ViewableArray.Pop();
    }
}

void SceneDescription::DeleteAllGammaStats()
{
    long i;
    for ( i=NumGammaStats(); i>0; i-- ) {
        delete GammaStatsArray.Pop();
    }
}
