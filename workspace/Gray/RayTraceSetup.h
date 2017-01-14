class VectorR3;
class Light;
class Material;
class ViewableBase;
class CameraView;
class SceneDescription;

// Camera and camera information

extern double Cpos[3];		// Position of camera
extern double Cdir[3];		// Direction of camera
extern double Cdist;		// Distance to "screen"
extern double Cdims[2];		// Width & height of "screen"

// Here are the arrays that hold information about the scene

extern SceneDescription TheScene2;

// Routines that load the data into the scene description:
void SetUpScene2();
void SetUpMainView();
void SetUpMaterials();
void SetUpLights( SceneDescription& scene );
void SetUpViewableObjects();
