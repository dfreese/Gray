#include <iostream>
#include <Graphics/SceneDescription.h>
#include <GraphicsTrees/IntersectionKdTree.h>
#include <Gray/GammaRayTrace.h>
#include <Gray/LoadMaterials.h>
#include <Gray/LoadDetector.h>
#include <Gray/Config.h>
#include <Random/Random.h>
#ifdef USE_OPENGL
#include <Viewer/Viewer.h>
#endif

using namespace std;

int main( int argc, char** argv)
{
    Config config;
    if (!config.ProcessCommandLine(argc,argv)) {
        Config::usage();
        return(-1);
    }

    GammaRayTrace Gray;
    SceneDescription FileScene;
    if (!LoadMaterials::LoadPhysicsFiles(FileScene)) {
        return(1);
    }
    if (!LoadDetector::Load(config.filename_detector, FileScene, Gray)) {
        cerr << "Loading file \"" << config.filename_detector << "\" failed" << endl;
        return(1);
    }
    Gray.SetFileNameOutput(config.filename_output);
    if (config.seed != 0) {
        Random::Seed(config.seed);
        cout << "Seeding Gray: " << config.seed << endl;
    }

    IntersectKdTree intersect_kd_tree(FileScene);
    Gray.SetKdTree(intersect_kd_tree);

    if (config.run_viewer_flag) {
#ifdef USE_OPENGL
        run_viewer(argc, argv, FileScene, intersect_kd_tree);
#endif
    }
    if (config.run_physics_flag) {
        Gray.GRayTraceSources();
    }
    return(0);
}
