#include <iostream>
#include <unordered_set>
#include <Graphics/SceneDescription.h>
#include <GraphicsTrees/IntersectionKdTree.h>
#include <Gray/GammaMaterial.h>
#include <Gray/GammaRayTrace.h>
#include <Gray/LoadMaterials.h>
#include <Gray/LoadDetector.h>
#include <Gray/Config.h>
#include <Output/DetectorArray.h>
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
    DetectorArray detector_array;
    SceneDescription scene;
    SourceList sources;
    if (!LoadMaterials::LoadPhysicsFiles(scene)) {
        return(1);
    }
    if (!LoadDetector::Load(config.get_filename_scene(), scene, sources,
                            config, detector_array))
    {
        cerr << "Loading file \"" << config.get_filename_scene()
             << "\" failed" << endl;
        return(1);
    }
    if (config.get_seed_set()) {
        Random::Seed(config.get_seed());
    } else {
        Random::Seed();
    }
    cout << "Using Seed: " << Random::GetSeed() << endl;

    // Setup the singles processor and load a default or specified mapping file
    SinglesStream<Interaction> singles_stream(
            5 * scene.GetMaxDistance() * Interaction::inverse_speed_of_light);

    if (config.get_filename_mapping() == "") {
        singles_stream.set_mappings(detector_array.default_mapping());
    } else if (singles_stream.load_mappings(config.get_filename_mapping()) < 0)
    {
        cerr << "Loading mapping file \"" << config.get_filename_mapping()
        << "\" failed" << endl;
        return(2);
    }

    if (config.get_filename_pipeline() == "") {
        singles_stream.set_processes(config.get_pipeline_lines());
    } else if (singles_stream.load_processes(config.get_filename_pipeline()) < 0)
    {
        cerr << "Loading pipeline file \"" << config.get_filename_pipeline()
             << "\" failed" << endl;
        return(3);
    }

    if (!config.get_run_viewer() && !config.get_run_physics()) {
        cout << "Neither viewer nor physics selected.  Exiting." << endl;
        return(0);
    }

    IntersectKdTree intersect_kd_tree(scene);
    sources.SetKdTree(intersect_kd_tree);


    if (config.get_run_viewer()) {
#ifdef USE_OPENGL
        run_viewer(argc, argv, scene, intersect_kd_tree);
#endif
    }

    if (!config.get_log_hits() && !config.get_log_singles()) {
        cout << "No output specified.  Exiting." << endl;
    }

    if (config.get_run_physics()) {

        Output output_hits;
        Output output_singles;
        if (config.get_log_hits()) {
            output_hits.SetLogfile(config.get_filename_hits());
            output_hits.SetFormat(config.get_format_hits());
        }
        if (config.get_log_singles()) {
            output_singles.SetLogfile(config.get_filename_singles());
            output_singles.SetFormat(config.get_format_singles());
        }

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
                    output_hits.GetLogPositron(), config.get_log_hits(),
                    config.get_log_hits(), stats);
            if (config.get_log_hits()) {
                for (const auto & interact: interactions) {
                    output_hits.LogInteraction(interact);
                }
            }
            if (config.get_log_singles()) {
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
        if (config.get_log_singles()) {
            singles_stream.stop();
            for (const auto & interact: singles_stream.get_ready()) {
                output_singles.LogInteraction(interact);
            }
            singles_stream.clear();
        }
        cout << "=] Done." << endl;
        cout << "\n______________\n Stats\n______________\n" << stats << endl;
        cout << "______________\n DAQ Stats\n______________\n"
             << singles_stream << endl;
    }
    return(0);
}
