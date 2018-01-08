#include <Gray/Simulation.h>
#include <Gray/Config.h>
#include <Gray/GammaRayTrace.h>
#include <Gray/GammaRayTraceStats.h>
#include <Gray/Mpi.h>
#include <Daq/DaqModel.h>
#include <Sources/SourceList.h>
#include <Graphics/SceneDescription.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

Simulation::Simulation(
        const Config& config,
        const SceneDescription& scene,
        const SourceList& sources,
        const DaqModel& daq_model,
        size_t thread_idx, size_t no_threads) :
    config(config),
    sources(sources),
    scene(scene),
    daq_model(daq_model),
    thread_idx(thread_idx),
    no_threads(no_threads),
    outputs_coinc(daq_model.no_coinc_processes())
{
    this->sources.SetSimulationTime(config.get_time());
    this->sources.SetStartTime(config.get_start_time());
    if (no_threads > 1) {
        this->sources.AdjustTimeForSplit(thread_idx, no_threads);
    }
    this->sources.InitSources();

    string output_append;
    if (no_threads > 1) {
        output_append = ".rank_" + std::to_string(thread_idx);
    }

    bool success = true;
    if (config.get_log_hits()) {
        output_hits.SetFormat(config.get_format_hits());
        output_hits.SetVariableOutputMask(
                config.get_hits_var_output_write_flags());
        success &= output_hits.SetLogfile(
                    config.get_filename_hits() + output_append);
    }
    if (config.get_log_singles()) {
        output_singles.SetFormat(config.get_format_singles());
        output_singles.SetVariableOutputMask(
                config.get_singles_var_output_write_flags());
        success &= output_singles.SetLogfile(
                config.get_filename_singles() + output_append);
    }
    if (config.get_log_coinc()) {
        for (size_t idx = 0; idx < outputs_coinc.size(); idx++) {
            Output & output_coinc = outputs_coinc[idx];
            output_coinc.SetFormat(config.get_format_coinc());
            output_coinc.SetVariableOutputMask(
                    config.get_coinc_var_output_write_flags());
            success &= output_coinc.SetLogfile(
                    config.get_filename_coinc(idx) + output_append);
        }
    }

    if (!success) {
        throw std::runtime_error("Unable to open output files");
    }
}

void Simulation::Run() {
    bool print_prog_bar = !Mpi::Enabled();
    const long num_chars = 70;
    double tick_mark = sources.GetSimulationTime() / num_chars;
    int current_tick = 0;

    GammaRayTrace ray_tracer(scene, sources.GetSourcePositions(),
                             config.get_log_nondepositing_inter(),
                             config.get_log_nuclear_decays(),
                             config.get_log_nonsensitive(),
                             config.get_log_nointeraction(),
                             config.get_log_errors());

    if (print_prog_bar) cout << "[" << flush;

    GammaRayTraceStats ray_stats;
    const size_t interactions_soft_max = 100000;
    daq_model.get_buffer().reserve(interactions_soft_max + 50);
    while (sources.SimulationIncomplete()) {
        while (sources.SimulationIncomplete()) {
            daq_model.consume(ray_tracer.TraceDecay(sources.Decay(), ray_stats));
            if (interactions_soft_max < daq_model.get_buffer().size()) {
                break;
            }
        }
        daq_model.process_hits();
        if (config.get_log_hits()) {
            output_hits.LogHits(daq_model.hits_begin(), daq_model.hits_end());
        }

        daq_model.process_singles();
        if (config.get_log_singles() || config.get_log_coinc()) {
            if (config.get_log_singles()) {
                output_singles.LogSingles(daq_model.singles_begin(),
                                          daq_model.singles_end());
            }

            for (size_t idx = 0; idx < daq_model.no_coinc_processes(); idx++) {
                daq_model.process_coinc(idx);
                if (config.get_log_coinc()) {
                    outputs_coinc[idx].LogCoinc(daq_model.coinc_begin(),
                                                daq_model.coinc_end(),
                                                true);
                }
            }
        }

        daq_model.clear_complete();

        for (; current_tick < (sources.GetElapsedTime() / tick_mark);
             current_tick++)
        {
            if (print_prog_bar) cout << "=" << flush;
        }
    }

    daq_model.stop_hits();
    if (config.get_log_hits()) {
        output_hits.LogHits(daq_model.hits_begin(), daq_model.hits_end());
        output_hits.Close();
    }

    daq_model.stop_singles();
    if (config.get_log_singles() || config.get_log_coinc()) {
        if (config.get_log_singles()) {
            output_singles.LogSingles(daq_model.singles_begin(),
                                      daq_model.singles_end());
            output_singles.Close();
        }

        for (size_t idx = 0; idx < daq_model.no_coinc_processes(); idx++) {
            daq_model.stop_coinc(idx);
            if (config.get_log_coinc()) {
                outputs_coinc[idx].LogCoinc(daq_model.coinc_begin(),
                                            daq_model.coinc_end(), true);
                outputs_coinc[idx].Close();
            }
        }
    }
    if (print_prog_bar) cout << "=] Done." << endl;
    cout << "\n______________\n Stats\n______________\n"
         << ray_stats << endl;
    if (config.get_log_singles() || config.get_log_coinc()) {
        cout << "______________\n DAQ Stats\n______________\n"
        << daq_model << endl;
    }
    // A NoOp function if MPI is not enabled.
    Mpi::CombineFiles(config, output_hits, output_singles, outputs_coinc);
    Mpi::Finalize();
}

