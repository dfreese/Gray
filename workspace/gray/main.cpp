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
#include <Physics/Physics.h>
#include <Pipeline/InteractionStream.h>

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
    if (!sources.LoadIsotopes(config.get_isotopes_filename())) {
        cerr << "Unable to load isotopes file: \""
        << config.get_isotopes_filename() << "\"\n"
        << "Check GRAY_INCLUDE env variable or specify name with --iso"
        << endl;
        return(1);
    }
    if (!LoadMaterials::LoadPhysicsFiles(scene,
                                         config.get_materials_filename()))
    {
        cerr << "Unable to load materials file: \""
             << config.get_materials_filename() << "\"\n"
             << "Check GRAY_INCLUDE env variable or specify name with --mat"
             << endl;
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
    const double max_req_sort_time = (5 * scene.GetMaxDistance() *
                                      Physics::inverse_speed_of_light);
    InteractionStream singles_stream(max_req_sort_time);


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

    IntersectKdTree intersect_kd_tree(scene);
    sources.SetKdTree(intersect_kd_tree);


    if (!config.get_log_hits() && !config.get_log_singles()) {
        cout << "Warning: No output specified." << endl;
    }

    Output output_hits;
    Output output_singles;
    std::vector<Output> outputs_coinc(singles_stream.no_coinc_processes());
    if (config.get_log_hits()) {
        output_hits.SetFormat(config.get_format_hits());
        output_hits.SetVariableOutputMask(config.get_hits_var_output_write_flags());
        output_hits.SetLogfile(config.get_filename_hits());
    }
    if (config.get_log_singles()) {
        output_singles.SetFormat(config.get_format_singles());
        output_singles.SetVariableOutputMask(config.get_singles_var_output_write_flags());
        output_singles.SetLogfile(config.get_filename_singles());
    }
    if (config.get_log_coinc()) {
        if (singles_stream.no_coinc_processes() !=
            config.get_no_coinc_filenames())
        {
            cerr << "Incorrect number of filenames specified for coinc outputs"
                 << endl;
            return(4);
        }
        for (size_t idx = 0; idx < singles_stream.no_coinc_processes(); idx++) {
            Output & output_coinc = outputs_coinc[idx];
            output_coinc.SetFormat(config.get_format_coinc());
            output_coinc.SetVariableOutputMask(config.get_coinc_var_output_write_flags());
            output_coinc.SetLogfile(config.get_filename_coinc(idx));
        }
    }

    sources.SetSimulationTime(config.get_time());
    sources.SetStartTime(config.get_start_time());
    sources.InitSources();

    // We only want to send certain interaction types into the singles
    // processor.
    unordered_set<int> singles_valid_interactions({
        Physics::COMPTON,
        Physics::PHOTOELECTRIC,
        Physics::XRAY_ESCAPE,
        Physics::RAYLEIGH});

    // TODO: check if time actually increases inside of negative sources
    // more than it should according to this comment below:
    // FIXME: time should not increase when Inside() of a negative source

    const long num_chars = 70;
    double tick_mark = sources.GetSimulationTime() / num_chars;
    int current_tick = 0;
    cout << "[" << flush;

    GammaRayTrace::TraceStats stats;
    const size_t interactions_soft_max = 100000;
    while (sources.GetTime() < sources.GetEndTime()) {
        vector<Interaction> interactions;
        interactions.reserve(interactions_soft_max + 50);
        GammaRayTrace::TraceSources(
                sources, intersect_kd_tree, interactions,
                interactions_soft_max,
                dynamic_cast<GammaMaterial*>(&scene.GetMaterial(0)),
                config.get_log_nuclear_decays(),
                config.get_log_nonsensitive(),
                config.get_log_nointeraction(),
                config.get_log_errors(), stats);
        if (config.get_log_hits()) {
            for (const auto & interact: interactions) {
                output_hits.LogInteraction(interact);
            }
        }
        if (config.get_log_singles() || config.get_log_coinc()) {
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
            auto singles_events = singles_stream.add_events(interactions);
            if (config.get_log_singles()) {
                for (const auto & interact: singles_events) {
                    output_singles.LogInteraction(interact);
                }
            }
            for (size_t idx = 0; idx < singles_stream.no_coinc_processes(); idx++) {
                // We need to make sure that we clear the coinc buffers every
                // so often (every round here) otherwise, they will build up
                // data.  A singles_stream.clear(), or a get_coinc_buffer call
                // to each buffer is required.
                auto coinc_events = singles_stream.get_coinc_buffer(idx);
                if (config.get_log_coinc()) {
                    for (const auto & interact: coinc_events) {
                        outputs_coinc[idx].LogInteraction(interact);
                    }
                }
            }
        }
        for (; current_tick < (sources.GetElapsedTime() / tick_mark);
             current_tick++)
        {
            cout << "=" << flush;
        }
    }
    if (config.get_log_singles()) {
        auto singles_events = singles_stream.stop();
        for (const auto & interact: singles_events) {
            output_singles.LogInteraction(interact);
        }
        for (size_t idx = 0; idx < singles_stream.no_coinc_processes(); idx++) {
            auto coinc_events = singles_stream.get_coinc_buffer(idx);
            if (config.get_log_coinc()) {
                for (const auto & interact: coinc_events) {
                    outputs_coinc[idx].LogInteraction(interact);
                }
            }
        }
    }
    cout << "=] Done." << endl;
    cout << "\n______________\n Stats\n______________\n" << stats << endl;
    if (config.get_log_singles() || config.get_log_coinc()) {
        cout << "______________\n DAQ Stats\n______________\n"
             << singles_stream << endl;
    }
    return(0);
}
