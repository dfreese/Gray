#include <iostream>
#include <Graphics/SceneDescription.h>
#include <GraphicsTrees/IntersectionKdTree.h>
#include <Gray/GammaMaterial.h>
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
    SceneDescription scene;
    if (!LoadMaterials::LoadPhysicsFiles(scene)) {
        return(1);
    }
    if (!LoadDetector::Load(config.filename_detector, scene, Gray.output, Gray.sources)) {
        cerr << "Loading file \"" << config.filename_detector << "\" failed" << endl;
        return(1);
    }
    Gray.output.SetLogfile(config.filename_output);
    Gray.SetDefaultMaterial(dynamic_cast<GammaMaterial*>(&scene.GetMaterial(0)));
    if (config.seed != 0) {
        Random::Seed(config.seed);
        cout << "Seeding Gray: " << config.seed << endl;
    }

    IntersectKdTree intersect_kd_tree(scene);
    Gray.SetKdTree(intersect_kd_tree);

    if (config.run_viewer_flag) {
#ifdef USE_OPENGL
        run_viewer(argc, argv, scene, intersect_kd_tree);
#endif
    }
    if (config.run_physics_flag) {
        Gray.GRayTraceSources();
    }
    return(0);
}
