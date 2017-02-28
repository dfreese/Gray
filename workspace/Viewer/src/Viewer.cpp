#include <Viewer/Viewer.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <Graphics/SceneDescription.h>
#include <GraphicsTrees/IntersectionKdTree.h>
#include <GraphicsTrees/ShadowKdTree.h>
#include <Viewer/PixelArray.h>

namespace {
    // Window size and pixel array variables
    int WindowWidth = 10;	// Width in pixels
    int WindowHeight = 10;	// Height in pixels
    PixelArray pixels(10,10); // Array of pixels

    // Next two variables can be used to keep from re-raytracing a window.
    long NumScanLinesRayTraced = -1;
    long WidthRayTraced = -1;

    SceneDescription* ActiveScene;
    ShadowKdTree * ActiveShadowKdTree;
    IntersectKdTree * ActiveIntersectKdTree;

    void CalcAllDirectIllum( const VectorR3& viewPos,
                            const VisiblePoint& visPoint,
                            VectorR3& returnedColor,
                            ShadowKdTree & kd_tree,
                            long avoidK = -1)
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
                clearpath = kd_tree.ShadowFeeler(visPoint.GetPosition(), thisLight, avoidK );
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

    void RayTrace(int TraceDepth, const VectorR3& pos, const VectorR3 dir,
                  VectorR3& returnedColor, IntersectKdTree & intersect_kd_tree,
                  ShadowKdTree & shadow_kd_tree, long avoidK = -1)
    {
        double hitDist;
        VisiblePoint visPoint;

        int intersectNum = intersect_kd_tree.SeekIntersection(pos, dir, &hitDist,
                                                              visPoint, avoidK);
        if ( intersectNum<0 ) {
            returnedColor = ActiveScene->BackgroundColor();
        } else {
            CalcAllDirectIllum(pos, visPoint, returnedColor, shadow_kd_tree, intersectNum);
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
                    RayTrace(TraceDepth - 1, visPoint.GetPosition(), nextDir,
                             moreColor, intersect_kd_tree, shadow_kd_tree,
                             intersectNum);
                    moreColor.x *= c.x;
                    moreColor.y *= c.y;
                    moreColor.z *= c.z;
                    returnedColor += moreColor;
                }

                // Ray Trace Transmission
                if ( thisMat->IsTransmissive() ) {
                    if ( thisMat->CalcRefractDir(visPoint.GetNormal(),dir, nextDir) ) {
                        VectorR3 c = thisMat->GetTransmissionColor(visPoint, -dir, nextDir);
                        RayTrace(TraceDepth - 1, visPoint.GetPosition(), nextDir,
                                 moreColor, intersect_kd_tree, shadow_kd_tree,
                                 intersectNum);
                        moreColor.x *= c.x;
                        moreColor.y *= c.y;
                        moreColor.z *= c.z;
                        returnedColor += moreColor;
                    }
                }
            }
        }
    }

    // called when the window is resized
    static void ResizeWindow(GLsizei w, GLsizei h)
    {
        h = (h==0) ? 1 : h;
        w = (w==0) ? 1 : w;

        WindowHeight = h;
        WindowWidth = w;
        if ( pixels.SetSize( WindowWidth, WindowHeight ) ) {	// If pixel data reallocated,
            NumScanLinesRayTraced = WidthRayTraced = -1;		// signal pixel data no longer valid
        }

        glViewport(0, 0, w, h);

        // Resize the camera viewpoint
        ActiveScene->CalcNewScreenDims( (double)w / (double)h );
        ActiveScene->GetCameraView().SetScreenPixelSize(pixels.GetWidth(), pixels.GetHeight());
    }

    // *******************************************************************
    // Handle all "normal" ascii key presses.
    // The "g" or space bar commands have the same effect: switch between
    //		rendering with OpenGL and rendering with Glut.
    // *******************************************************************
    void myKeyboardFunc( unsigned char key, int x, int y )
    {
        switch ( key ) {
            case 'q':
            case 'Q':
                cout << "Quit."  << endl;
                throw 0;
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
                // Signal view has changed
                NumScanLinesRayTraced = WidthRayTraced = -1;
                glutPostRedisplay();
                break;
            case GLUT_KEY_DOWN:
                ActiveScene->GetCameraView().RotateViewUp( -0.1 );
                // Signal view has changed
                NumScanLinesRayTraced = WidthRayTraced = -1;
                glutPostRedisplay();
                break;
            case GLUT_KEY_RIGHT:
                ActiveScene->GetCameraView().RotateViewRight( 0.1 );
                // Signal view has changed
                NumScanLinesRayTraced = WidthRayTraced = -1;
                glutPostRedisplay();
                break;
            case GLUT_KEY_LEFT:
                ActiveScene->GetCameraView().RotateViewRight( -0.1 );
                // Signal view has changed
                NumScanLinesRayTraced = WidthRayTraced = -1;
                glutPostRedisplay();
                break;
            case GLUT_KEY_HOME:
                ActiveScene->GetCameraView().RescaleDistanceOfViewer( 1.1 );
                // Signal view has changed
                NumScanLinesRayTraced = WidthRayTraced = -1;
                glutPostRedisplay();
                break;
            case GLUT_KEY_END:
                ActiveScene->GetCameraView().RescaleDistanceOfViewer( 0.9 );
                // Signal view has changed
                NumScanLinesRayTraced = WidthRayTraced = -1;
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
            cout << "Mouse click at: " << x << " " << y << endl;
        }
    }
}

// *****************************************************************
// RayTraceView() is the top level routine that starts the ray tracing.
//	Current implementation: casts a ray to the center of each pixel.
//	Calls RayTrace() for each one.  This must be declared extern to keep it in
// the global scope to be used as the main loop for OpenGL
// *****************************************************************
extern void RayTraceView(void)
{
    int i,j;
    VectorR3 PixelDir;
    VisiblePoint visPoint;
    VectorR3 curPixelColor;		// Accumulator for Pixel Color

    const CameraView& MainView = ActiveScene->GetCameraView();

    if ( WidthRayTraced!=WindowWidth || NumScanLinesRayTraced!=WindowHeight ) {
        // Do the rendering here
        ActiveShadowKdTree->ResetStats();
        ActiveIntersectKdTree->ResetStats();
        int TraceDepth = 12;
        for ( i=0; i<WindowWidth; i++) {
            for ( j=0; j<WindowHeight; j++ ) {
                //i = 249;
                //j = WindowHeight-183;
                MainView.CalcPixelDirection(i,j,&PixelDir);
                RayTrace(TraceDepth, MainView.GetPosition(), PixelDir,
                         curPixelColor, *ActiveIntersectKdTree,
                         *ActiveShadowKdTree);
                pixels.SetPixel(i,j,curPixelColor);
            }
        }
        // Set these values to show scene has been computed.
        WidthRayTraced = WindowWidth;
        NumScanLinesRayTraced = WindowHeight;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WindowWidth, 0, WindowHeight);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    pixels.Draw();

    // flush the pipeline, swap the buffers
    glFlush();
    glutSwapBuffers();

}

void run_viewer(int argc, char** argv,
                SceneDescription & FileScene,
                IntersectKdTree & intersect_kd_tree)
{
    ActiveScene = &FileScene;
    
    ActiveScene->GetCameraView().SetScreenPixelSize(pixels.GetWidth(), pixels.GetHeight());
    ActiveScene->RegisterCameraView();
    ShadowKdTree shadow_kd_tree(FileScene);
    ActiveShadowKdTree = &shadow_kd_tree;
    ActiveIntersectKdTree = &intersect_kd_tree;
    
    cout << "Arrow keys change view direction (and use OpenGL)." << endl;
    cout << "Home/End keys alter view distance --- resizing keeps it same view size." << endl;
    cout << "Press 'Q' or 'q' to Quit" << endl;
    
    glutInit(&argc, argv);
    // we're going to animate it, so double buffer
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize(301,246);
    glutInitWindowPosition(0, 0);
    glutCreateWindow( "Ray Tracing" );
    
    // set up callback functions
    glutKeyboardFunc( myKeyboardFunc );
    glutSpecialFunc( mySpecialFunc );
    glutMouseFunc( myMouseUpDownFunc );
    glutReshapeFunc( ResizeWindow );
    
    // call this in main loop
    glutDisplayFunc(RayTraceView);
    try {
        glutMainLoop();
    } catch (...) {
        cout << "exit command caught" << endl;
    }
    
}
