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
    if (config.log_singles) {
        if (singles_stream.load_mappings(config.filename_mapping) < 0) {
            cerr << "Loading mapping file \"" << config.filename_mapping
                 << "\" failed" << endl;
        }
        if (singles_stream.load_processes(config.filename_pipeline) < 0) {
            cerr << "Loading pipeline file \"" << config.filename_pipeline
                 << "\" failed" << endl;
        }
    }

    if (!config.run_viewer_flag && !config.run_physics_flag) {
        cout << "Neither viewer nor physics selected.  Exiting." << endl;
        return(0);
    }

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

        // TODO: check if time actually increases inside of negative sources
        // more than it should according to this comment below:
        // FIXME: time should not increase when Inside() of a negative source

        const long num_chars = 70;
        double tick_mark = sources.GetSimulationTime() / num_chars;
        int current_tick = 0;
        cout << "[" << flush;

        GammaRayTrace::TraceStats stats;
        const size_t interactions_soft_max = 100000;
        vector<Interaction> interactions;
        interactions.reserve(interactions_soft_max + 50);
        while (sources.GetTime() < sources.GetSimulationTime()) {
            interactions.clear();
            GammaRayTrace::TraceSources(
                    sources, intersect_kd_tree, interactions,
                    interactions_soft_max,
                    dynamic_cast<GammaMaterial*>(&scene.GetMaterial(0)),
                    output_hits.GetLogPositron(), config.log_hits,
                    config.log_hits, stats);
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
            for (; current_tick < (sources.GetTime() / tick_mark); current_tick++) {
                cout << "=" << flush;
            }
        }
        if (config.log_singles) {
            singles_stream.stop();
            for (const auto & interact: singles_stream.get_ready()) {
                output_singles.LogInteraction(interact);
            }
            singles_stream.clear();
        }
        cout << "=] Done." << endl;
        cout << "______________\n Stats\n______________\n" << stats << endl;
    }
    return(0);
}
