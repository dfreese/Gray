#include <Viewer.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <Graphics/SceneDescription.h>
#include <GlutRenderer.h>

namespace {

SceneDescription* ActiveScene;

// called when the window is resized
static void ResizeWindow(GLsizei w, GLsizei h)
{
    h = (h==0) ? 1 : h;
    w = (w==0) ? 1 : w;

    glViewport(0, 0, w, h);

    // Resize the camera viewpoint
    ActiveScene->CalcNewScreenDims( (double)w / (double)h );
    ActiveScene->GetCameraView().SetScreenPixelSize(w, h);
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
            glutPostRedisplay();
            break;
        case GLUT_KEY_DOWN:
            ActiveScene->GetCameraView().RotateViewUp( -0.1 );
            glutPostRedisplay();
            break;
        case GLUT_KEY_RIGHT:
            ActiveScene->GetCameraView().RotateViewRight( 0.1 );
            glutPostRedisplay();
            break;
        case GLUT_KEY_LEFT:
            ActiveScene->GetCameraView().RotateViewRight( -0.1 );
            glutPostRedisplay();
            break;
        case GLUT_KEY_HOME:
            ActiveScene->GetCameraView().RescaleDistanceOfViewer( 1.1 );
            glutPostRedisplay();
            break;
        case GLUT_KEY_END:
            ActiveScene->GetCameraView().RescaleDistanceOfViewer( 0.9 );
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

extern void OpenGLRender() {
    GlutRenderer glut;
    glut.RenderScene(*ActiveScene);
}

/*
 * Note on viewer:
 *
 * Decided to drop having the viewer work on mac to be able use strictly OpenGL
 * for the viewer, and make it independent of the kd-tree implementation.  The
 * current implementation of the OpenGL viewer has a significant number of
 * problems as it is wildly out of date.  It relies on GLUT and GLU for a
 * number of its primitive drawing.  Additionally it's entirely based on the
 * OpenGL fixed pipeline.  All of the viewing would need to be reimplemented
 * using a more modern version of OpenGL using shaders (vertex/fragment).
 * While doing this, it would be opportune to switch viewers to something less
 * crude that GLUT.
 * -Freese
 */
void run_viewer(int argc, char** argv, SceneDescription & FileScene) {
    ActiveScene = &FileScene;
    ActiveScene->RegisterCameraView();

    cout << "Arrow keys change view direction (and use OpenGL)." << endl;
    cout << "Home/End keys alter view distance --- resizing keeps it same view size." << endl;
    cout << "Press 'Q' or 'q' to Quit" << endl;

    glutInit(&argc, argv);
    // we're going to animate it, so double buffer
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize(301,246);
    glutInitWindowPosition(0, 0);
    int window_id = glutCreateWindow( "Ray Tracing" );

    // set up callback functions
    glutKeyboardFunc( myKeyboardFunc );
    glutSpecialFunc( mySpecialFunc );
    glutMouseFunc( myMouseUpDownFunc );
    glutReshapeFunc( ResizeWindow );

    // call this in main loop
    glutDisplayFunc(OpenGLRender);
    try {
        glutMainLoop();
    } catch (...) {
        cout << "exit command caught" << endl;
    }
    glutDestroyWindow(window_id);
}
