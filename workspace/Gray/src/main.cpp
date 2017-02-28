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

#include <Gray/Config.h>
#include <PixelArray/PixelArray.h>
#include <Graphics/ViewableBase.h>
#include <Graphics/DirectLight.h>
#include <Graphics/CameraView.h>
#include <KdTree/KdTree.h>
#include <Graphics/SceneDescription.h>
#include <Gray/GammaRayTrace.h>
#include <Gray/LoadMaterials.h>
#include <Random/Random.h>
#include <Gray/LoadDetector.h>
#include <GraphicsTrees/IntersectionKdTree.h>
#include <GraphicsTrees/ShadowKdTree.h>
#include <string>
#include <sstream>
#include <Gray/Viewer.h>

bool InitializeSceneGeometry(GammaRayTrace & Gray,
                             const Config & config,
                             SceneDescription & FileScene)
{
    // Define the lights, materials, textures and viewable objects.
    if (!LoadMaterials::LoadPhysicsFiles(FileScene)) {
        exit(-1);
    }
    LoadDetector myLoader;
    if (!myLoader.Load(config.filename_detector, FileScene, Gray)) {
        fprintf(stderr, "Loading file \"%s\" failed\n",
                config.filename_detector.c_str());
    }
    Gray.SetFileNameOutput(config.filename_output);
    if (config.seed != 0) {
        Random::Seed(config.seed);
        printf("Seeding Gray: %ld\n", config.seed);
    }
    return(true);
}

//**********************************************************
// Main Routine
// Set up OpenGL, hook up callbacks, define RayTrace world,
// and start the main loop
//**********************************************************
int main( int argc, char** argv)
{
    Config config;
    if (!config.ProcessCommandLine(argc,argv)) {
        Config::usage();
        return(-1);
    }

    GammaRayTrace Gray;
    SceneDescription FileScene;
    if (!InitializeSceneGeometry(Gray, config, FileScene)) {
        return(1);
    }
    IntersectKdTree intersect_kd_tree(FileScene);
    Gray.SetKdTree(intersect_kd_tree);

    if (config.batch_mode == true) {
        // Implement batch mode for raytracing
        Gray.GRayTraceSources();
        return(0);
    }
    run_viewer(argc, argv, FileScene, intersect_kd_tree);
    return(0);
}
