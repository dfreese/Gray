#include "Gray/Gray/Simulation.h"
#include "Gray/Gray/Config.h"
#include "Gray/Gray/GammaRayTrace.h"
#include "Gray/Gray/GammaRayTraceStats.h"
#include "Gray/Daq/DaqModel.h"
#include "Gray/Sources/SourceList.h"
#include "Gray/Graphics/SceneDescription.h"
#include <cstdio>
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
    outputs_coinc(daq_model.no_coinc_processes()),
    sources(sources),
    daq_model(daq_model),
    thread_idx(thread_idx),
    scene(scene),
    config(config)
{
    if (no_threads > 1) {
        this->sources.AdjustTimeForSplit(thread_idx, no_threads);
    }
    this->sources.InitSources();

    string output_append;
    if (no_threads > 1) {
        output_append = ".rank_" + std::to_string(thread_idx);
    }

    // For the files not written by the first process, don't write the header
    // out so the files can simply be concatenated.  Since that doesn't decode
    // what is in the files, currently, the decay_id starts over for every
    // thread.
    bool write_header = (thread_idx == 0);

    bool success = true;
    if (config.get_log_hits()) {
        output_hits.SetFormat(config.get_format_hits());
        output_hits.SetVariableOutputMask(
                config.get_hits_var_output_write_flags());
        success &= output_hits.SetLogfile(
                    config.get_filename_hits() + output_append,
                    write_header);
    }
    if (config.get_log_singles()) {
        output_singles.SetFormat(config.get_format_singles());
        output_singles.SetVariableOutputMask(
                config.get_singles_var_output_write_flags());
        success &= output_singles.SetLogfile(
                config.get_filename_singles() + output_append,
                write_header);
    }
    if (config.get_log_coinc()) {
        for (size_t idx = 0; idx < outputs_coinc.size(); idx++) {
            Output & output_coinc = outputs_coinc[idx];
            output_coinc.SetFormat(config.get_format_coinc());
            output_coinc.SetVariableOutputMask(
                    config.get_coinc_var_output_write_flags());
            success &= output_coinc.SetLogfile(
                    config.get_filename_coinc(idx) + output_append,
                    write_header);
        }
    }

    if (!success) {
        throw std::runtime_error("Unable to open output files");
    }
}

SimulationStats Simulation::Run() {
    bool print_prog_bar = (thread_idx == 0);
    const long num_chars = 70;
    double tick_mark = sources.GetSimulationTime() / num_chars;
    int current_tick = 0;

    GammaRayTrace ray_tracer(scene, sources.GetSourcePositions(),
                             config.get_log_nondepositing_inter(),
                             config.get_log_nuclear_decays(),
                             config.get_log_nonsensitive(),
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
    SimulationStats result;
    result.physics = ray_stats;
    result.daq = daq_model.stats();
    return (result);
}

void Simulation::CombineOutputs(
        const Config& config, const std::vector<Simulation>& sims)
{
    if (sims.size() == 1) {
        return;
    }
    std::ofstream hits;
    std::ofstream singles;
    // Unclear why g++ was complaining about emplace_back with ofstream,
    // but this works...
    std::vector<unique_ptr<ofstream>> coincs;
    if (config.get_log_hits()) {
        hits.open(config.get_filename_hits());
    }
    if (config.get_log_singles()) {
        singles.open(config.get_filename_singles());
    }
    if (config.get_log_coinc()) {
        for (const auto& name: config.get_filenames_coinc()) {
            coincs.emplace_back(new std::ofstream(name));
        }
    }
    for (int idx = 0; idx < sims.size(); idx++) {
        const Simulation& sim = sims[idx];
        if (config.get_log_hits()) {
            std::ifstream hits_seg(sim.output_hits.GetFilename());
            hits << hits_seg.rdbuf();
            hits_seg.close();
            std::remove(sim.output_hits.GetFilename().c_str());
        }
        if (config.get_log_singles()) {
            std::ifstream singles_seg(sim.output_singles.GetFilename());
            singles << singles_seg.rdbuf();
            singles_seg.close();
            std::remove(sim.output_singles.GetFilename().c_str());
        }
        if (config.get_log_coinc()) {
            for (size_t coinc_idx = 0; coinc_idx < config.get_no_coinc_filenames(); ++coinc_idx) {
                std::ifstream coinc_seg(sim.outputs_coinc[coinc_idx].GetFilename());
                (*coincs[coinc_idx]) << coinc_seg.rdbuf();
                coinc_seg.close();
                std::remove(sim.outputs_coinc[coinc_idx].GetFilename().c_str());
            }
        }
    }
}
