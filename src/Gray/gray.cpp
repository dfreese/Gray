/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include <ctime>
#include <future>
#include <iostream>
#include <vector>
#include "Gray/Graphics/SceneDescription.h"
#include "Gray/Gray/GammaMaterial.h"
#include "Gray/Gray/GammaRayTrace.h"
#include "Gray/Gray/LoadMaterials.h"
#include "Gray/Gray/Load.h"
#include "Gray/Gray/Config.h"
#include "Gray/Gray/Simulation.h"
#include "Gray/Output/DetectorArray.h"
#include "Gray/Output/Output.h"
#include "Gray/Sources/SourceList.h"
#include "Gray/Random/Random.h"
#include "Gray/Physics/Physics.h"
#include "Gray/Daq/DaqModel.h"

using namespace std;

int main(int argc, char ** argv) {
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

    Load load;
    if (!load.File(config.get_filename_scene(), sources, scene,
                detector_array, config))
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

    sources.SetSimulationTime(config.get_time());
    sources.SetStartTime(config.get_start_time());
    // Allow a world size and rank to be set via the command line, as well as
    // by the number of threads internally.
    if (config.get_world_size() > 1) {
        sources.AdjustTimeForSplit(config.get_rank(), config.get_world_size());
    }
    if (config.get_print_splits()) {
        if (sources.PrintSplits(config.get_no_threads())) {
            return (0);
        } else {
            return (6);
        }
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

    if (config.get_log_coinc() &&
            (daq_model.no_coinc_processes() != config.get_no_coinc_filenames()))
    {
        std::cerr << "Incorrect number of filenames specified for coinc outputs\n";
        return(4);
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

    // We add an offset proportional to the rank within the world to make sure
    // that each node would run a different simulation, assuming this was begin
    // run on a cluster with each node receving a unique -r/--rank id.  We also
    // take into account how many threads each simulation will run and leave
    // space for each of the threads to index from there in the same manner.
    Random::SetSeed(config.get_seed() +
            (config.get_rank() * config.get_no_threads()));
    cout << "Using Seed: " << Random::GetSeed() << endl;

    int no_threads = config.get_no_threads();
    std::vector<Simulation> sims;
    for (int idx = 0; idx < no_threads; ++idx) {
        sims.emplace_back(Simulation(config, scene, sources, daq_model, idx, no_threads));
    }
    clock_t setup_time = clock();
    std::vector<std::future<SimulationStats>> results(no_threads);
    for (int idx = 0; idx < no_threads; ++idx) {
        // For the first simulation which will be the only one run for a single
        // thread environment, use the deferred launch policy which will run
        // in this thread.  For all others, use async to launch another thread.
        auto policy = idx == 0 ? std::launch::deferred:std::launch::async;
        // equivalent to Simulation sim(); sim.Run(); in separate threads.
        results[idx] = std::async(policy, &Simulation::Run, &sims[idx]);
    }
    SimulationStats total;
    for (auto& r : results) {
        total += r.get();
    }

    Simulation::CombineOutputs(config, sims);

    cout << "\n______________\n Stats\n______________\n"
         << total.physics << endl;
    if (config.get_log_singles() || config.get_log_coinc()) {
        cout << "______________\n DAQ Stats\n______________\n"
        << total.daq << endl;
    }

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
