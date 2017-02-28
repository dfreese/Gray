#include <iostream>
#include <Graphics/SceneDescription.h>
#include <GraphicsTrees/IntersectionKdTree.h>
#include <Gray/GammaRayTrace.h>
#include <Gray/LoadMaterials.h>
#include <Gray/LoadDetector.h>
#include <Gray/Config.h>
#include <Viewer/Viewer.h>
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
        return(1);
    }
    Gray.SetFileNameOutput(config.filename_output);
    if (config.seed != 0) {
        Random::Seed(config.seed);
        cout << "Seeding Gray: " << config.seed << endl;
    }

    IntersectKdTree intersect_kd_tree(FileScene);
    Gray.SetKdTree(intersect_kd_tree);

    if (!config.batch_mode) {
        run_viewer(argc, argv, FileScene, intersect_kd_tree);
    }
    Gray.GRayTraceSources();
    return(0);
}
