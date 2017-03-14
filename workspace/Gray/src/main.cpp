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
        int num_decays_total = sources.GetTotalEvents();
        int num_decays_cur = 0;

        const int num_chars = 70;
        int tick_mark = (int)(num_decays_total / num_chars);
        if (tick_mark == 0) {
            // Make sure we don't have an error later on because of num % 0
            tick_mark = 1;
        }
        int current_tick = 0;
        cout << "[";
        while (num_decays_cur < num_decays_total) {
            vector<Interaction> interactions;
            interactions.reserve(10000);
            num_decays_cur += GammaRayTrace::TraceSources(
                    sources, intersect_kd_tree,
                    num_decays_total - num_decays_cur, interactions,
                    interactions.capacity(),
                    dynamic_cast<GammaMaterial*>(&scene.GetMaterial(0)));
            for (const auto & interact: interactions) {
                output.LogInteraction(interact);
            }
            for (; current_tick < (num_decays_cur / tick_mark); current_tick++) {
                cout << "=";
            }
            cout.flush();
        }
        cout << "=] Done." << endl;
    }
    return(0);
}
