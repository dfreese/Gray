#include <iostream>
#include <Graphics/SceneDescription.h>
#include <GraphicsTrees/IntersectionKdTree.h>
#include <Gray/GammaMaterial.h>
#include <Gray/GammaRayTrace.h>
#include <Gray/LoadMaterials.h>
#include <Gray/LoadDetector.h>
#include <Gray/Config.h>
#include <Output/Output.h>
#include <Sources/SourceList.h>
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

    SceneDescription scene;
    Output output;
    SourceList sources;
    if (!LoadMaterials::LoadPhysicsFiles(scene)) {
        return(1);
    }
    if (!LoadDetector::Load(config.filename_detector, scene, output, sources)) {
        cerr << "Loading file \"" << config.filename_detector << "\" failed" << endl;
        return(1);
    }
    output.SetLogfile(config.filename_output);
    if (config.seed != 0) {
        Random::Seed(config.seed);
        cout << "Seeding Gray: " << config.seed << endl;
    }

    IntersectKdTree intersect_kd_tree(scene);
    sources.SetKdTree(intersect_kd_tree);

    if (config.run_viewer_flag) {
#ifdef USE_OPENGL
        run_viewer(argc, argv, scene, intersect_kd_tree);
#endif
    }
    if (config.run_physics_flag) {

        // calculate the number of positrons to throw
        // TODO: need to fix the number of rays because of negative sources
        // FIXME: time should not increase when Inside() of a negative source
        int num_decays = sources.GetTotalEvents();
        GammaRayTrace::TraceSources(sources, output, intersect_kd_tree,
                                    num_decays,
                                    dynamic_cast<GammaMaterial*>(&scene.GetMaterial(0)));
    }
    return(0);
}
