#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <Gray/RayTraceStats.h>
#include <PixelArray/PixelArray.h>
#include <Graphics/ViewableBase.h>
#include <Graphics/DirectLight.h>
#include <Graphics/CameraView.h>
#include <OpenGLRender/GlutRenderer.h>
#include <Graphics/KdTree.h>
#include <Graphics/SceneDescription.h>
#include <Gray/GammaRayTrace.h>
#include <Gray/LoadMaterials.h>
#include <Random/Random.h>
#include <Gray/LoadDetector.h>


void RenderWithGlut(void);

void RayTraceView(void);
long SeekIntersection(const VectorR3& startPos, const VectorR3& direction,
                      double *hitDist, VisiblePoint& returnedPoint,
                      long avoidK);
long SeekIntersectionKd(const VectorR3& startPos, const VectorR3& direction,
                        double *hitDist, VisiblePoint& returnedPoint,
                        long avoidK);
void RayTrace( int TraceDepth, const VectorR3& pos, const VectorR3 dir,
               VectorR3& returnedColor, long avoidK = -1);
bool ShadowFeeler(const VectorR3& pos, const Light& light, long intersectNum=-1 );
void CalcAllDirectIllum( const VectorR3& viewPos, const VisiblePoint& visPoint,
                         VectorR3& returnedColor, long avoidK = -1);

static void ResizeWindow(GLsizei w, GLsizei h);

// ***********************Statistics************
RayTraceStats MyStats;
// **********************************************

GammaRayTrace Gray;
const char * FileNameDetector = "default.dff";
const char * FileNameOutput = "default.dat";
unsigned long GraySeed = 0;
bool BatchMode = false;


// Window size and pixel array variables
bool WindowMinimized = false;
int WindowWidth;	// Width in pixels
int WindowHeight;	// Height in pixels
PixelArray* pixels;		// Array of pixels

bool RayTraceMode = false;		// Set true for RayTraciing,  false for rendering with OpenGL
// Rendering with OpenGL does not support all features, esp., texture mapping
// Next two variables can be used to keep from re-raytracing a window.

bool GammaRayTraceMode = false;

long NumScanLinesRayTraced = -1;
long WidthRayTraced = -1;

SceneDescription* ActiveScene;

SceneDescription FileScene;			// Scene that is loaded from an .obj or .nff file.


void InitLightsAndView(GlutRenderer & glut, const SceneDescription & scene)
{
    // Set camera position
    glut.SetBackgroundColor( scene.BackgroundColor() );
    glut.SetupCameraView( scene.GetCameraView() );
    // Define all light sources
    for (int i = 0; i < scene.NumLights(); i++) {
        glut.AddLight(scene.GetLight(i));
    }
    glut.SetGlobalAmbientLight(scene.GlobalAmbientLight());
}

void RenderViewables(GlutRenderer & glut, const SceneDescription & scene)
{
    glut.RenderViewables(scene.GetViewableArray());
}

void RenderScene(GlutRenderer & glut, const SceneDescription & scene)
{
    InitLightsAndView(glut, scene);
    RenderViewables(glut, scene);
    glut.FinishRendering();
}


// RenderScene() chooses between using OpenGL or  ray-tracing to render the scene
static void RenderScene(void)
{
    if ( WindowMinimized ) {
        return;
    }
    if ( RayTraceMode ) {
        RayTraceView();
    } else if ( GammaRayTraceMode ) {
        Gray.GRayTraceSources();
        GammaRayTraceMode = false;
    } 	else {
        GlutRenderer newGlutter;
        RenderScene(newGlutter, *ActiveScene);
    }
}

// ******************************************************
//   KdTree definitions and routines for creating the KdTree
// ******************************************************
KdTree ObjectKdTree;

void myExtentFunc( long objNum, AABB& retBox )
{
    ActiveScene->GetViewable(objNum).CalcAABB( retBox );
}
bool myExtentsInBox( long objNum, const AABB& aabb, AABB& retBox)
{
    return ActiveScene->GetViewable(objNum).CalcExtentsInBox( aabb, retBox );
}

void myBuildKdTree()
{
    ObjectKdTree.SetDoubleRecurseSplitting( true );
    ObjectKdTree.SetObjectCost(8.0);
    ObjectKdTree.BuildTree( ActiveScene->NumViewables(), myExtentFunc, myExtentsInBox  );
    RayTraceStats::PrintKdStats( ObjectKdTree );
}

// *****************************************************************
// RayTraceView() is the top level routine that starts the ray tracing.
//	Current implementation: casts a ray to the center of each pixel.
//	Calls RayTrace() for each one.
// *****************************************************************

void RayTraceView(void)
{
    int i,j;
    VectorR3 PixelDir;
    VisiblePoint visPoint;
    VectorR3 curPixelColor;		// Accumulator for Pixel Color

    const CameraView& MainView = ActiveScene->GetCameraView();

    if ( WidthRayTraced!=WindowWidth || NumScanLinesRayTraced!=WindowHeight ) {
        // Do the rendering here
        MyStats.Init();
        ObjectKdTree.ResetStats();
        int TraceDepth = 12;
        for ( i=0; i<WindowWidth; i++) {
            for ( j=0; j<WindowHeight; j++ ) {
                //i = 249;
                //j = WindowHeight-183;
                MainView.CalcPixelDirection(i,j,&PixelDir);
                RayTrace( TraceDepth, MainView.GetPosition(), PixelDir, curPixelColor );
                pixels->SetPixel(i,j,curPixelColor);
            }
        }
        WidthRayTraced = WindowWidth;			// Set these values to show scene has been computed.
        NumScanLinesRayTraced = WindowHeight;
        MyStats.GetKdRunData( ObjectKdTree );
        MyStats.PrintStats();
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WindowWidth, 0, WindowHeight);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    pixels->Draw();

    // flush the pipeline, swap the buffers
    glFlush();
    glutSwapBuffers();

}


// *********************************************************
// Data that supports the callback operation
//		of the SeekIntersectionKd kd-Tree Traversal
//      and the ShadowFeelerKd kd-Tree Traversal
// *********************************************************
bool kdTraverseFeeler;
double isectEpsilon = 1.0e-6;
//double isectEpsilon = 1.0e-18;
long bestObject;
long kdTraverseAvoid;
double bestHitDistance;
double kdShadowDist;
VisiblePoint tempPoint;
VisiblePoint* bestHitPoint;
VectorR3 kdStartPos;
VectorR3 kdStartPosAvoid;
VectorR3 kdTraverseDir;

// Call back function for KdTraversal of view ray or reflection ray
// It is of type PotentialObjectCallback.
bool potHitSeekIntersection( long objectNum, double* retStopDistance )
{
    double thisHitDistance;
    bool hitFlag;

    if (GammaRayTraceMode) {
        if (ActiveScene->GetViewable(objectNum).GammaReject()) {
            return false;
        }
    }

    if ( objectNum == kdTraverseAvoid ) {
        hitFlag = ActiveScene->GetViewable(objectNum).FindIntersection(kdStartPosAvoid, kdTraverseDir,
                  bestHitDistance, &thisHitDistance, tempPoint);
        if ( !hitFlag ) {
            return false;
        }
        thisHitDistance += isectEpsilon;		// Adjust back to real hit distance
    } else {
        hitFlag = ActiveScene->GetViewable(objectNum).FindIntersection(kdStartPos, kdTraverseDir,
                  bestHitDistance, &thisHitDistance, tempPoint);
        if ( !hitFlag ) {
            return false;
        }
    }

    *bestHitPoint = tempPoint;		// The visible point that was hit
    bestObject = objectNum;				// The object that was hit
    bestHitDistance = thisHitDistance;
    *retStopDistance = bestHitDistance;	// No need to traverse search further than this distance
    return true;
}

// Call back function for KdTraversal of shadow feeler
// It is of type PotentialObjectCallback.
bool potHitShadowFeeler( long objectNum, double* retStopDistance )
{
    double thisHitDistance;
    bool hitFlag = ActiveScene->GetViewable(objectNum).FindIntersection(kdStartPos, kdTraverseDir,
                   kdShadowDist, &thisHitDistance, tempPoint);
    if  ( hitFlag && !(/*objectNum==kdTraverseAvoid &&*/ thisHitDistance+isectEpsilon>=kdShadowDist) ) {
        kdTraverseFeeler = false;
        *retStopDistance = -1.0;	// Negative value should abort process quickly
        return true;
    } else {
        return false;
    }
}



// SeekIntersectionKd seeks for an intersection with all viewable objects
// If it finds one, it returns the index of the viewable object,
//   and sets the value of hitDist and fills in the returnedPoint values.
// This "Kd" version uses the Kd-Tree
long SeekIntersectionKd(const VectorR3& pos, const VectorR3& direction,
                        double *hitDist, VisiblePoint& returnedPoint,
                        long avoidK)
{
    MyStats.AddRayTraced();

    bestObject = -1;
    bestHitDistance = DBL_MAX;
    kdTraverseAvoid = avoidK;
    kdStartPos = pos;
    kdTraverseDir = direction;
    kdStartPosAvoid = pos;
    kdStartPosAvoid.AddScaled( direction, isectEpsilon );
    bestHitPoint = &returnedPoint;

    bestObject = -1;
    ObjectKdTree.Traverse( pos, direction, *potHitSeekIntersection );

    if ( bestObject>=0 ) {
        // FIXME: NAN in KDTREE Traversal
        if(isnan(bestHitDistance)) {
            *hitDist = DBL_MAX;
            bestObject = -1;
        } else {
            *hitDist = bestHitDistance;
        }
    }
    return bestObject;
}

// ShadowFeeler - returns whether the light is visible from the position pos.
//		Return value is "true" if no shadowing object found.
//		intersectNum is the index of the visible object being (possibly)
//		illuminated at pos.

bool ShadowFeelerKd(const VectorR3& pos, const Light& light, long intersectNum )
{
    MyStats.AddRayTraced();
    MyStats.AddShadowFeeler();

    kdTraverseDir = pos;
    kdTraverseDir -= light.GetPosition();
    double dist = kdTraverseDir.Norm();
    if ( dist<1.0e-7 ) {
        return true;		// Extremely close to the light!
    }
    kdTraverseDir /= dist;			// Direction from light position towards pos
    kdStartPos = light.GetPosition();
    kdTraverseFeeler = true;		// True indicates no shadowing objects
    kdTraverseAvoid = intersectNum;
    kdShadowDist = dist;
    ObjectKdTree.Traverse( light.GetPosition(), kdTraverseDir, potHitShadowFeeler, dist, true );

    return kdTraverseFeeler;	// Return whether ray is free of shadowing objects
}


void RayTrace( int TraceDepth, const VectorR3& pos, const VectorR3 dir,
               VectorR3& returnedColor, long avoidK )
{
    double hitDist;
    VisiblePoint visPoint;

    int intersectNum = SeekIntersectionKd(pos,dir,
                                          &hitDist,visPoint, avoidK );
    if ( intersectNum<0 ) {
        returnedColor = ActiveScene->BackgroundColor();
    } else {
        CalcAllDirectIllum( pos, visPoint, returnedColor, intersectNum );
        if ( TraceDepth > 1 ) {
            VectorR3 nextDir;
            VectorR3 moreColor;
            const MaterialBase* thisMat = &(visPoint.GetMaterial());

            // Ray trace reflection
            if ( thisMat->IsReflective() ) {
                nextDir = visPoint.GetNormal();
                nextDir *= -2.0*(dir^visPoint.GetNormal());
                nextDir += dir;
                nextDir.ReNormalize();	// Just in case...
                VectorR3 c = thisMat->GetReflectionColor(visPoint, -dir, nextDir);
                RayTrace( TraceDepth-1, visPoint.GetPosition(), nextDir, moreColor, intersectNum);
                moreColor.x *= c.x;
                moreColor.y *= c.y;
                moreColor.z *= c.z;
                returnedColor += moreColor;
            }

            // Ray Trace Transmission
            if ( thisMat->IsTransmissive() ) {
                if ( thisMat->CalcRefractDir(visPoint.GetNormal(),dir, nextDir) ) {
                    VectorR3 c = thisMat->GetTransmissionColor(visPoint, -dir, nextDir);
                    RayTrace( TraceDepth-1, visPoint.GetPosition(), nextDir, moreColor, intersectNum);
                    moreColor.x *= c.x;
                    moreColor.y *= c.y;
                    moreColor.z *= c.z;
                    returnedColor += moreColor;
                }
            }
        }
#ifdef DEBUG_RAYTRACE
        if ((returnedColor.x == ActiveScene->BackgroundColor().x) &&
                (returnedColor.y == ActiveScene->BackgroundColor().y) &&
                (returnedColor.z == ActiveScene->BackgroundColor().z)) {
            // bad things in little china
            printf("Bad things in little china\n");
        }
#endif
    }
}

void CalcAllDirectIllum( const VectorR3& viewPos,
                         const VisiblePoint& visPoint,
                         VectorR3& returnedColor, long avoidK )
{
    const MaterialBase* thisMat = &(visPoint.GetMaterial());
    const VectorR3& ambientcolor = thisMat->GetColorAmbient();
    const VectorR3& ambientlight = ActiveScene->GlobalAmbientLight();
    const VectorR3& emitted = thisMat->GetColorEmissive();
    returnedColor.x = ambientcolor.x*ambientlight.x + emitted.x;
    returnedColor.y = ambientcolor.y*ambientlight.y + emitted.y;
    returnedColor.z = ambientcolor.z*ambientlight.z + emitted.z;

    VectorR3 thisColor;
    VectorR3 percentLit;
    VectorR3 toLight;
    bool checksides = visPoint.GetMaterial().IsTransmissive();
    double viewDot;
    if ( !checksides ) {						// If not transmissive
        toLight = viewPos;
        toLight -= visPoint.GetPosition();		// Direction to *viewer*
        viewDot = toLight^visPoint.GetNormal();
    }
    bool clearpath;

    int numLights = ActiveScene->NumLights();
    for ( int k=0; k<numLights; k++ ) {
        const Light& thisLight = ActiveScene->GetLight(k);
        clearpath = true;
        // Cast a shadow feeler if (a) transmissive or (b) light and view on the same side
        if ( !checksides ) {
            toLight = thisLight.GetPosition();
            toLight -= visPoint.GetPosition();		// Direction to light
            if ( !SameSignNonzero( viewDot, (toLight^visPoint.GetNormal()) ) ) {
                clearpath = false;
            }
        }
        if ( clearpath ) {
            clearpath = ShadowFeelerKd(visPoint.GetPosition(), thisLight, avoidK );
        }
        if ( clearpath ) {
            percentLit.Set(1.0,1.0,1.0);	// Directly lit, with no shadowing
        } else {
            percentLit.SetZero();	// Blocked by shadows (still do ambient lighting)
        }
        DirectIlluminateViewPos (visPoint, viewPos,
                                 thisLight, thisColor, percentLit);
        returnedColor.x += thisColor.x;
        returnedColor.y += thisColor.y;
        returnedColor.z += thisColor.z;
    }
}

// called when the window is resized
static void ResizeWindow(GLsizei w, GLsizei h)
{
    WindowMinimized = (h==0 || w==0);
    h = (h==0) ? 1 : h;
    w = (w==0) ? 1 : w;

    if ( (NumScanLinesRayTraced!=h || WidthRayTraced!=w) && !WindowMinimized ) {
        RayTraceMode = false;							// Go back to OpenGL mode if size changes.
    }

    WindowHeight = h;
    WindowWidth = w;
    if ( pixels->SetSize( WindowWidth, WindowHeight ) ) {	// If pixel data reallocated,
        RayTraceMode = false;
        NumScanLinesRayTraced = WidthRayTraced = -1;		// signal pixel data no longer valid
    }

    glViewport(0, 0, w, h);

    // Resize the camera viewpoint
    ActiveScene->CalcNewScreenDims( (double)w / (double)h );
    ActiveScene->GetCameraView().SetScreenPixelSize(pixels->GetWidth(), pixels->GetHeight());
}

// *******************************************************************
// Handle all "normal" ascii key presses.
// The "g" or space bar commands have the same effect: switch between
//		rendering with OpenGL and rendering with Glut.
// *******************************************************************
void myKeyboardFunc( unsigned char key, int x, int y )
{
    switch ( key ) {

    case 'g':							// "g" command
    case ' ':							// Space bar
        // Set to be in Ray Trace mode
        if ( !RayTraceMode ) {
            RayTraceMode = true;
            fprintf(stdout,"Rendering start\n");
            glutPostRedisplay();
        }
        break;
    case 'G':							// 'G' command
        // Set to be rendering with OpenGL
        if ( RayTraceMode ) {
            RayTraceMode = false;
            glutPostRedisplay();
        }
        break;
    case 'P':
    case 'p':
        fprintf(stdout,"Starting physics simulation\n");
        GammaRayTraceMode = true;
        glutPostRedisplay();
        break;
    case 'q':
    case 'Q':
        fprintf(stdout,"Quit.\n");
        exit(0);
        break;
    }
}

// *******************************************************************
// Handle all "special" key presses.
// *******************************************************************
void mySpecialFunc( int key, int x, int y )
{
    switch ( key ) {

    case GLUT_KEY_UP:
        ActiveScene->GetCameraView().RotateViewUp( 0.1 );
        RayTraceMode = false;
        NumScanLinesRayTraced = WidthRayTraced = -1;	// Signal view has changed
        glutPostRedisplay();
        break;
    case GLUT_KEY_DOWN:
        ActiveScene->GetCameraView().RotateViewUp( -0.1 );
        RayTraceMode = false;
        NumScanLinesRayTraced = WidthRayTraced = -1;	// Signal view has changed
        glutPostRedisplay();
        break;
    case GLUT_KEY_RIGHT:
        ActiveScene->GetCameraView().RotateViewRight( 0.1 );
        RayTraceMode = false;
        NumScanLinesRayTraced = WidthRayTraced = -1;	// Signal view has changed
        glutPostRedisplay();
        break;
    case GLUT_KEY_LEFT:
        ActiveScene->GetCameraView().RotateViewRight( -0.1 );
        RayTraceMode = false;
        NumScanLinesRayTraced = WidthRayTraced = -1;	// Signal view has changed
        glutPostRedisplay();
        break;
    case GLUT_KEY_HOME:
        ActiveScene->GetCameraView().RescaleDistanceOfViewer( 1.1 );
        RayTraceMode = false;
        NumScanLinesRayTraced = WidthRayTraced = -1;	// Signal view has changed
        glutPostRedisplay();
        break;
    case GLUT_KEY_END:
        ActiveScene->GetCameraView().RescaleDistanceOfViewer( 0.9 );
        RayTraceMode = false;
        NumScanLinesRayTraced = WidthRayTraced = -1;	// Signal view has changed
        glutPostRedisplay();
        break;
    }
}

// **********************************************************
// Called for mouse clicks  (Debugging mostly)
//***********************************************************
void myMouseUpDownFunc( int button, int state, int x, int y )
{
    if ( button!=GLUT_LEFT_BUTTON ) {
        return;
    }
    if ( state==GLUT_DOWN ) {
        fprintf(stdout, "Mouse click at: %d, %d.\n", x, y );
    }
}

void InitializeSceneGeometry()
{
    // Define the lights, materials, textures and viewable objects.
    if (!LoadMaterials::LoadPhysicsFiles(FileScene)) {
        exit(-1);
    }
    LoadDetector myLoader;
    if (!myLoader.Load(FileNameDetector, FileScene, Gray)) {
        fprintf(stderr, "Loading file \"%s\" failed\n", FileNameDetector);
    }
    Gray.SetFileNameOutput(FileNameOutput);
    if (GraySeed != 0) {
        Random::Seed(GraySeed);
        printf("Seeding Gray: %ld\n",GraySeed);
    }
    ActiveScene = &FileScene;

    pixels = new PixelArray(10,10);		// Array of pixels
    ActiveScene->GetCameraView().SetScreenPixelSize(pixels->GetWidth(), pixels->GetHeight());
    ActiveScene->RegisterCameraView();

    // Build the kd-Tree.
    myBuildKdTree();
}

const char * switch_batch = "-b";
const char * switch_seed = "-seed";
bool SeedParse = false;
bool setFilenameDetector = false;
bool setFilenameOutput = false;

bool GrayProcessCommandLine(int argc, char **argv)
{
//    BatchMode = true;
//    GraySeed = 1;
////    FileNameDetector = "../../../detectors/neg_sphere.dff";
//    FileNameDetector = "/Users/david/Desktop/normalization/simulation/breast_panel.dff";
//    FileNameOutput = "neg_sphere.dat";
//    setFilenameDetector = true;
//    setFilenameOutput = true;
//    return true;
    
    if (argc == 1) {
        fprintf(stdout, "Syntax:\n\tGray [-b] [-seed num] [FileNameDetector] [FileNameOutput]\n");
    } else {
        for (int index = 1; index < argc; index++) {
            //printf("\nINDEX: %d: %s\n",index,argv[index]);
            if (strcmp(argv[index],switch_batch)==0) {
                BatchMode = true;
            } else if (strcmp(argv[index],switch_seed)==0) {
                SeedParse = true;
            } else if (isdigit(argv[index][0])) {
                sscanf(argv[index],"%ld",&GraySeed);
            } else {
                if (setFilenameDetector == false) {
                    FileNameDetector = argv[index];
                    setFilenameDetector = true;
                } else if (setFilenameOutput == false) {
                    FileNameOutput = argv[index];
                    setFilenameOutput = true;
                }
            }
        }
    }
    return true;
}

//**********************************************************
const char * GRAY_VERSION = "BETA_2_010";
//**********************************************************


//**********************************************************
// Main Routine
// Set up OpenGL, hook up callbacks, define RayTrace world,
// and start the main loop
//**********************************************************
int main( int argc, char** argv )
{
    fprintf( stdout, "\nGRAY VERSION: ");
    fprintf( stdout, "%s", GRAY_VERSION);
    fprintf( stdout, "\n\n");
    GrayProcessCommandLine(argc,argv);

    if (BatchMode == true) {
        // Implement batch mode for raytracing
        InitializeSceneGeometry();
        Gray.GRayTraceSources();
        exit(0);
    }

    fprintf( stdout, "Press 'g' or space bar to start ray tracing. (And then wait!)\n" );
    fprintf( stdout, "Press 'G' to return to OpenGL render mode.\n" );
    fprintf( stdout, "Arrow keys change view direction (and use OpenGL).\n" );
    fprintf( stdout, "Home/End keys alter view distance --- resizing keeps it same view size.\n");

    fprintf( stdout, "Press 'P' or 'p' to ray trace Physics\n");
    fprintf( stdout, "Press 'Q' or 'q' to Quit\n");

    glutInit(&argc, argv);
    // we're going to animate it, so double buffer
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize(301,246);
    glutInitWindowPosition(0, 0);
    glutCreateWindow( "Ray Tracing" );

    InitializeSceneGeometry();

    // set up callback functions
    glutKeyboardFunc( myKeyboardFunc );
    glutSpecialFunc( mySpecialFunc );
    glutMouseFunc( myMouseUpDownFunc );
    glutReshapeFunc( ResizeWindow );

    // call this in main loop
    glutDisplayFunc(RenderScene);
    glutMainLoop();

    return(0);
}
