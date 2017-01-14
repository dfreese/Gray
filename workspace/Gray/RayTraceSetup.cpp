#include "RayTraceSetup.h"

#include "../Graphics/Material.h"
#include "../Graphics/Light.h"
#include "../Graphics/CameraView.h"
#include "../Gray/SceneDescription.h"

#ifdef GAMMA_PHYSICS
#include "../Gray/GammaRayTrace.h"
extern GammaRayTrace Gray;
#endif

// ******************************************

SceneDescription TheScene2;			// The scene as created by the routines below

// ******************************************

// Some simple colors

double RedColor[3] = {1.0, 0.0, 0.0};
double BlueColor[3] = {0.0, 0.0, 1.0};
double BlackColor[3] = {0.0, 0.0, 0.0};

// Camera Information Data  *****************************************

#define SCENE_WIDTH		200.0 //20.0
#define SCENE_HEIGHT	200.0
#define SCENE_DEPTH		200.0

CameraView* MainView;
double Cpos[3] = {0.0,SCENE_HEIGHT/2.0,0.8*SCENE_DEPTH};	// Position of camera
double Cdir[3] = {0.0,0.0,-1.0};				// Direction of camera
double Cdist = 0.8*SCENE_DEPTH;					// Distance to "screen"
double Cdims[2] = {SCENE_WIDTH, SCENE_HEIGHT};	// Width & height of "screen"


// Use next two to focus on the tori
//double Cdims[2] = {2.0,2.0};		// Width & height of "screen"
//double Cdir[3] = {0.0,-4.9,-20.0};	// Direction of camera

// Data for lights  **********************************************

// Global lighting data

// Lighting values
float Lt0amb[3] = {0.3f, 0.3f, 0.3f};
float Lt0diff[3] = {1.0f, 1.0f, 1.0f};
float Lt0spec[3] = {1.0f, 1.0f, 1.0f};
float Lt0pos[3] = {-0.8*SCENE_WIDTH, -0.9*SCENE_HEIGHT, -SCENE_DEPTH/2.0};

float Lt1amb[3] = {0.0f, 0.0f, 0.0f};
float Lt1diff[3] = {1.0f, 1.0f, 1.0f};
float Lt1spec[3] = {1.0f, 1.0f, 1.0f};
float Lt1pos[3] = {0.8*SCENE_WIDTH, 0.9*SCENE_HEIGHT, -4.0};

// Data for materials *******************************************

// Material values

// First material for the floor
float Mat0spec[3] = {0.8f, 0.8f, 0.8f};
float Mat0nonspec[3] = {0.5f, 0.5f, 0.0f};
float Mat0reflect[3] = {0.0f, 0.0f, 0.0f};
float Mat0shiny = 512.0f;

// Second material for the floor
float Mat1spec[3] = {0.8f, 0.8f, 0.8f};
float Mat1nonspec[3] = {0.8f, 0.2f, 0.8f};
float Mat1reflect[3] = {0.0f, 0.0f, 0.0f};
float Mat1shiny = 512.0f;

// Material for the side walls
float Mat2spec[3] = {0.2f, 0.2f, 0.2f};
float Mat2nonspec[3] = {0.3f, 0.3f, 0.0f};
float Mat2reflect[3] = {0.0f, 0.0f, 0.0f};
float Mat2shiny = 160.0f;

// Red, mixed with some blue, (i.e., magenta) slightly reflective
float Mat3spec[3] = {0.7f, 0.7f, 0.7f};
float Mat3nonspec[3] = {0.9f, 0.0f, 0.6f};
float Mat3reflect[3] = {0.1f, 0.1f, 0.1f};
float Mat3shiny = 512.0f;

float Mat4spec[3] = {1.0f, 1.0f, 1.0f};
float Mat4nonspec[3] = {0.5f, 0.0f, 0.0f};
float Mat4reflect[3] = {0.1f, 0.1f, 0.1f};
float Mat4shiny = 512.0f;

float Mat5spec[3] = {0.6f, 0.6f, 0.6f};
float Mat5nonspec[3] = {0.0f, 0.0f, 0.0f};
float Mat5reflect[3] = {0.3f, 0.3f, 0.3f};
float Mat5trans[3] = {0.8f, 0.8f, 0.8f};
float Mat5shiny = 512.0f;

float Mat6spec[3] = {0.2f, 0.2f, 0.2f};
float Mat6nonspec[3] = {0.0f, 0.2f, 0.8f};
float Mat6reflect[3] = {0.3f, 0.3f, 0.3f};
float Mat6shiny = 160.0f;

// Black!
float Mat7spec[3] = {0.6f, 0.6f, 0.6f};
float Mat7nonspec[3] = {0.0f, 0.0f, 0.0f};
float Mat7reflect[3] = {0.0f, 0.0f, 0.0f};
float Mat7shiny = 160.0f;

// Completely invisible!
float Mat8spec[3] = {0.0f, 0.0f, 0.0f};
float Mat8nonspec[3] = {0.0f, 0.0f, 0.0f};
float Mat8reflect[3] = {0.0f, 0.0f, 0.0f};
float Mat8trans[3] = {1.0f, 1.0f, 1.0f};

// A near perfect mirror
float Mat9spec[3] = {0.95f, 0.95f, 0.95f};
float Mat9nonspec[3] = {0.05f, 0.05f, 0.05f};
float Mat9reflect[3] = {0.95f, 0.95f, 0.95f};
float Mat9shiny = 160.0f;


// Data for Viewable Objects  ************************************

// Vertices of triangles & rectangles
float par0verts[3][3] = { {-8.0f,0.0f,10.0f}, {8.0f,0.0f,10.0f}, {8.0f,0.0f, -5.0f} };
float par1verts[3][3] = { {-8.0f,0.0f,-5.0f}, {8.0f,0.0f,-5.0f}, {8.0f,10.0f,-5.0f} };
float tri2verts[3][3] = { {-8.0f,0.0f,-5.0f}, {-8.0f,10.0f,-5.0f}, {-8.0f,0.0f,10.0f} };
float tri3verts[3][3] = { {8.0f,0.0f,-5.0f}, {8.0f,0.0f,10.0f}, {8.0f,10.0f,-5.0f} };

void SetUpScene2()
{
    SetUpMainView();
    SetUpMaterials();
    SetUpLights( TheScene2 );
    SetUpViewableObjects();
}

void SetUpMainView()
{
    // Set Up Camera Information
    TheScene2.GetCameraView().SetPosition( Cpos );
    TheScene2.GetCameraView().SetDirection( Cdir );
    TheScene2.GetCameraView().SetScreenDistance( Cdist );
    TheScene2.GetCameraView().SetScreenDimensions( Cdims[0], Cdims[1] );
}

void SetUpMaterials()
{
    // Initialize Array of Materials
    Material* mat0 = new Material;
    TheScene2.AddMaterial( mat0 );
    mat0->SetColorAmbientDiffuse( Mat0nonspec );
    mat0->SetColorSpecular( Mat0spec );
    mat0->SetShininess( Mat0shiny );
    mat0->SetColorReflective( Mat0reflect );

    Material* mat1 = new Material;
    TheScene2.AddMaterial(mat1);
    mat1->SetColorAmbientDiffuse( Mat1nonspec );
    mat1->SetColorSpecular( Mat1spec );
    mat1->SetShininess( Mat1shiny );

    Material* mat2 = new Material;
    TheScene2.AddMaterial(mat2);
    mat2->SetColorAmbientDiffuse( Mat2nonspec );
    mat2->SetColorSpecular( Mat2spec );
    mat2->SetColorReflective( Mat2reflect );
    mat2->SetShininess( Mat2shiny );

    Material* mat3 = new Material;
    TheScene2.AddMaterial(mat3);
    mat3->SetColorAmbientDiffuse( Mat3nonspec );
    mat3->SetColorSpecular( Mat3spec );
    mat3->SetColorReflective( Mat3reflect );
    mat3->SetShininess( Mat3shiny );

    Material* mat4 = new Material;
    TheScene2.AddMaterial(mat4);
    mat4->SetColorAmbientDiffuse( Mat4nonspec );
    mat4->SetColorSpecular( Mat4spec );
    mat4->SetColorReflective( Mat4reflect );
    mat4->SetShininess( Mat4shiny );

    Material* mat5 = new Material;
    TheScene2.AddMaterial(mat5);
    mat5->SetColorAmbientDiffuse( Mat5nonspec );
    mat5->SetColorSpecular( Mat5spec );
    mat5->SetColorReflective( Mat5reflect );
    mat5->SetColorTransmissive( Mat5trans );
    mat5->SetShininess( Mat5shiny );
    mat5->SetIndexOfRefraction(1.5);	// Glass!

    Material* mat6 = new Material;
    TheScene2.AddMaterial(mat6);
    mat6->SetColorAmbientDiffuse( Mat6nonspec );
    mat6->SetColorSpecular( Mat6spec );
    mat6->SetColorReflective( Mat6reflect );
    mat6->SetShininess( Mat6shiny );

    Material* mat7 = new Material;
    TheScene2.AddMaterial(mat7);
    mat7->SetColorAmbientDiffuse( Mat7nonspec );
    mat7->SetColorSpecular( Mat7spec );
    mat7->SetColorReflective( Mat7reflect );
    mat7->SetShininess( Mat7shiny );

    // Perfectly invisible with index of
    //		refraction = 0, reflection = 0, transmission = 1.
    //	Use for two facing pieces of glass.
    Material* mat8 = new Material;
    TheScene2.AddMaterial(mat8);
    mat8->SetColorAmbientDiffuse(Mat8nonspec);
    mat8->SetColorSpecular(Mat8spec);
    mat8->SetColorReflective( Mat8reflect );
    mat8->SetColorTransmissive( Mat8trans );

    // A near perfect mirror surface
    Material* mat9 = new Material;
    TheScene2.AddMaterial(mat9);
    mat9->SetColorAmbientDiffuse(Mat9nonspec);
    mat9->SetColorSpecular(Mat9spec);
    mat9->SetColorReflective( Mat9reflect );
    mat9->SetShininess( Mat9shiny );

}

void SetUpLights( SceneDescription& scene )
{
    // Global ambient light and the background color are set above.

    // Initialize Array of Lights
    Light* myLight0 = new Light();
    scene.AddLight( myLight0 );
    myLight0->SetColorAmbient( Lt0amb );
    myLight0->SetColorDiffuse( Lt0diff );
    myLight0->SetColorSpecular( Lt0spec );
    myLight0->SetPosition( Lt0pos );

    Light* myLight1 = new Light();
    scene.AddLight( myLight1 );
    myLight1->SetColorAmbient( Lt1amb );
    myLight1->SetColorDiffuse( Lt1diff );
    myLight1->SetColorSpecular( Lt1spec );
    myLight1->SetPosition( Lt1pos );

}


void SetUpViewableObjects()
{

#ifdef GAMMA_PHYSICS
#endif

}
