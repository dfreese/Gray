#include <ctime>
#include <iostream>
#include <Graphics/SceneDescription.h>
#include <Gray/GammaMaterial.h>
#include <Gray/GammaRayTrace.h>
#include <Gray/LoadMaterials.h>
#include <Gray/LoadDetector.h>
#include <Gray/Config.h>
#include <Gray/Simulation.h>
#include <Output/DetectorArray.h>
#include <Output/Output.h>
#include <Sources/SourceList.h>
#include <Random/Random.h>
#include <Physics/Physics.h>
#include <Daq/DaqModel.h>

using namespace std;

int gray(int argc, char ** argv)
{
    clock_t start_time = clock();
    Config config;
    int config_status = config.ProcessCommandLine(argc, argv, true);
    if (config_status < 0) {
        return(1);
    } else if (config_status > 0) {
        return(0);
    }

    DetectorArray detector_array;
    SceneDescription scene;
    SourceList sources;
    if (!sources.LoadIsotopes(config.get_physics_filename())) {
        cerr << "Unable to load physics file: \""
        << config.get_physics_filename() << "\"\n"
        << "Check GRAY_INCLUDE env variable or specify name with --phys"
        << endl;
        return(1);
    }
    if (!LoadMaterials::LoadPhysicsJson(scene, config.get_physics_filename()))
    {
        cerr << "Unable to load physics file: \""
        << config.get_physics_filename() << "\"\n"
        << "Check GRAY_INCLUDE env variable or specify name with --phys"
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

    // Setup the singles processor and load a default or specified mapping file
    const double max_req_sort_time = (5 * scene.GetMaxDistance() *
                                      Physics::inverse_speed_of_light);
    DaqModel daq_model(max_req_sort_time);

    if (config.get_write_pos()) {
        if (!detector_array.WritePositions(config.get_write_pos_filename())) {
            std::cerr << "Unable to open detector position file for writing: "
                      << config.get_write_pos_filename() << "\n";
            return(4);
        }
        std::cout << "Detector position file written to "
                  << config.get_write_pos_filename() << "\n";
    }
    if (config.get_write_map()) {
        if (!detector_array.WriteDefaultMapping(
                    config.get_write_map_filename()))
        {
            std::cerr << "Unable to write default mapping file: "
                      << config.get_write_map_filename() << "\n";
            return(5);
        }

        std::cout << "Mapping file written to "
        << config.get_write_map_filename() << "\n";
    }
    if (config.get_write_pos() || config.get_write_map()) {
        std::cout << "Exiting.\n";
        return (0);
    }

    if (!config.get_filename_mapping().empty()) {
        if (!detector_array.LoadMapping(config.get_filename_mapping())) {
            cerr << "Loading mapping file \"" << config.get_filename_mapping()
                 << "\" failed" << endl;
            return(2);
        }
    }

    if (config.get_filename_process().empty()) {
        daq_model.set_processes(config.get_process_lines(),
                                detector_array.Mapping());
    } else if (daq_model.load_processes(config.get_filename_process(),
                                        detector_array.Mapping()) < 0)
    {
        cerr << "Loading pipeline file \"" << config.get_filename_process()
             << "\" failed" << endl;
        return(3);
    }

    scene.BuildTree(true, 8.0);

    if (config.get_run_overlap_test()) {
        cout << "testing for overlapping geometries" << endl;
        if (scene.TestOverlap()) {
            cerr << "overlap test failed" << endl;
            return (4);
        } else {
            cout << "overlap test passed" << endl;
            return (0);
        }
    }

    if (!config.get_log_any()) {
        cout << "Warning: No output specified." << endl;
    }

    Random::SetSeed(config.get_seed());
    cout << "Using Seed: " << Random::GetSeed() << endl;

    Output output_hits;
    Output output_singles;
    std::vector<Output> outputs_coinc(daq_model.no_coinc_processes());
    int setup_status = Simulation::SetupOutput(config, output_hits,
                                               output_singles, outputs_coinc);
    if (setup_status < 0) {
        return(4);
    }
    Simulation::SetupSources(config, sources, scene);

    clock_t setup_time = clock();
    Simulation::RunSim(config, sources, scene, output_hits,
                       output_singles, outputs_coinc, daq_model);

    clock_t end_time = clock();
    double setup_time_sec =  double(setup_time - start_time) / CLOCKS_PER_SEC;
    double run_time_sec =  double(end_time - setup_time) / CLOCKS_PER_SEC;
    double full_time_sec =  double(end_time - start_time) / CLOCKS_PER_SEC;
    cout << "CPU Time (s):"
         << "  setup = " << setup_time_sec << ", "
         << "run = " << run_time_sec << ", "
         << "total = " << full_time_sec << endl;
    return(0);
}
