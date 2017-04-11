#include <iostream>
#include <unordered_set>
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
#include <Pipeline/singlesstream.h>
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
    SourceList sources;
    if (!LoadMaterials::LoadPhysicsFiles(scene)) {
        return(1);
    }
    if (!LoadDetector::Load(config.filename_scene, scene, sources)) {
        cerr << "Loading file \"" << config.filename_scene << "\" failed"
             << endl;
        return(1);
    }
    Output output_hits(config.filename_hits);
    Output output_singles(config.filename_output);
    if (config.seed_set) {
        Random::Seed(config.seed);
    }
    if (!Random::SeedSet()) {
        Random::Seed();
    }
    cout << "Using Seed: " << Random::GetSeed() << endl;

    SinglesStream<Interaction> singles_stream(
            5 * scene.GetMaxDistance() * Interaction::inverse_speed_of_light);

    IntersectKdTree intersect_kd_tree(scene);
    sources.SetKdTree(intersect_kd_tree);


    if (config.run_viewer_flag) {
#ifdef USE_OPENGL
        run_viewer(argc, argv, scene, intersect_kd_tree);
#endif
    }
    if (config.run_physics_flag) {

        // We only want to send certain interaction types into the singles
        // processor.
        unordered_set<int> singles_valid_interactions({
            Interaction::COMPTON,
            Interaction::PHOTOELECTRIC,
            Interaction::XRAY_ESCAPE,
            Interaction::RAYLEIGH});

        // calculate the number of positrons to throw
        // TODO: need to fix the number of rays because of negative sources
        // FIXME: time should not increase when Inside() of a negative source
        long num_decays_total = sources.GetTotalEvents();
        long num_decays_cur = 0;

        const long num_chars = 70;
        long tick_mark = num_decays_total / num_chars;
        if (tick_mark == 0) {
            // Make sure we don't have an error later on because of num % 0
            tick_mark = 1;
        }
        int current_tick = 0;
        cout << "[";
        const size_t interactions_soft_max = 100000;
        vector<Interaction> interactions;
        interactions.reserve(interactions_soft_max);
        while (num_decays_cur < num_decays_total) {
            interactions.clear();
            num_decays_cur += GammaRayTrace::TraceSources(
                    sources, intersect_kd_tree,
                    num_decays_total - num_decays_cur, interactions,
                    interactions_soft_max,
                    dynamic_cast<GammaMaterial*>(&scene.GetMaterial(0)),
                    output_hits.GetLogPositron());
            if (config.log_hits) {
                for (const auto & interact: interactions) {
                    output_hits.LogInteraction(interact);
                }
            }
            if (config.log_singles) {
                // Partition the interactions into two sets, while preserving
                // the rough time order.  Anything "true", or having a type
                // in the singles_valid_interactions set is put in the front
                // and the remainder is removed.
                auto del_pos = stable_partition(
                        interactions.begin(), interactions.end(),
                        [&singles_valid_interactions](const Interaction & i){
                             return(singles_valid_interactions.count(i.type) &&
                                    (i.det_id >= 0));
                         });
                interactions.resize(del_pos - interactions.begin());
                singles_stream.add_events(interactions);
                for (const auto & interact: singles_stream.get_ready()) {
                    output_singles.LogInteraction(interact);
                }
                singles_stream.clear();
            }
            for (; current_tick < (num_decays_cur / tick_mark); current_tick++) {
                cout << "=";
            }
            cout.flush();
        }
        if (config.log_singles) {
            singles_stream.stop();
            for (const auto & interact: singles_stream.get_ready()) {
                output_singles.LogInteraction(interact);
            }
            singles_stream.clear();
        }
        cout << "=] Done." << endl;
    }
    return(0);
}
