#include <iostream>
#include <Graphics/SceneDescription.h>
#include <GraphicsTrees/IntersectionKdTree.h>
#include <GraphicsTrees/ShadowKdTree.h>
#include <Gray/GammaRayTrace.h>
#include <Gray/LoadMaterials.h>
#include <Gray/LoadDetector.h>
#include <Gray/Config.h>
#include <Gray/Viewer.h>
#include <Random/Random.h>

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
    LoadDetector myLoader;
    if (!myLoader.Load(config.filename_detector, FileScene, Gray)) {
        cerr << "Loading file \"" << config.filename_detector << "\" failed" << endl;
    }
    Gray.SetFileNameOutput(config.filename_output);
    if (config.seed != 0) {
        Random::Seed(config.seed);
        cout << "Seeding Gray: " << config.seed << endl;
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
