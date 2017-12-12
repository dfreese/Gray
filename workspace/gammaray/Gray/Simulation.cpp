#include <Gray/Simulation.h>
#include <Gray/Config.h>
#include <Gray/GammaRayTrace.h>
#include <Gray/Mpi.h>
#include <Pipeline/InteractionStream.h>
#include <Random/Random.h>
#include <Sources/SourceList.h>
#include <Graphics/SceneDescription.h>
#include <iostream>
#include <vector>

using namespace std;

void Simulation::SetupSeed(const Config & config) {
    if (config.get_seed_set()) {
        Random::Seed(config.get_seed());
    } else {
        Random::Seed();
    }
    Mpi::ReSeed();
    cout << "Using Seed: " << Random::GetSeed() << endl;
}

void Simulation::SetupSources(const Config & config, SourceList & sources,
                              SceneDescription & scene)
{
    sources.SetSimulationTime(config.get_time());
    sources.SetStartTime(config.get_start_time());
    Mpi::AdjustSimTime(sources);
    sources.BuildMaterialStacks(scene);
    sources.InitSources();
}

int Simulation::SetupOutput(const Config & config, Output & output_hits,
                            Output & output_singles,
                            std::vector<Output> & outputs_coinc)
{
    Mpi::ConfigOutputFileHeaders();
    // This will be blank if mpi is not enabled.
    string mpi_output_append = Mpi::OutputAppend();
    if (config.get_log_hits()) {
        output_hits.SetFormat(config.get_format_hits());
        output_hits.SetVariableOutputMask(config.get_hits_var_output_write_flags());
        output_hits.SetLogfile(config.get_filename_hits() + mpi_output_append);
    }
    if (config.get_log_singles()) {
        output_singles.SetFormat(config.get_format_singles());
        output_singles.SetVariableOutputMask(config.get_singles_var_output_write_flags());
        output_singles.SetLogfile(config.get_filename_singles() + mpi_output_append);
    }
    if (config.get_log_coinc()) {
        if (outputs_coinc.size() != config.get_no_coinc_filenames())
        {
            cerr << "Incorrect number of filenames specified for coinc outputs"
                 << endl;
            return(-1);
        }
        for (size_t idx = 0; idx < outputs_coinc.size(); idx++) {
            Output & output_coinc = outputs_coinc[idx];
            output_coinc.SetFormat(config.get_format_coinc());
            output_coinc.SetVariableOutputMask(config.get_coinc_var_output_write_flags());
            output_coinc.SetLogfile(config.get_filename_coinc(idx) + mpi_output_append);
        }
    }
    return(0);
}

void Simulation::RunSim(const Config & config, SourceList & sources,
                        const SceneDescription & scene,
                        Output & output_hits, Output & output_singles,
                        std::vector<Output> & outputs_coinc,
                        InteractionStream & singles_stream)
{
    bool print_prog_bar = !Mpi::Enabled();
    const long num_chars = 70;
    double tick_mark = sources.GetSimulationTime() / num_chars;
    int current_tick = 0;

    GammaRayTrace ray_tracer(sources, scene,
                             config.get_log_nondepositing_inter(),
                             config.get_log_nuclear_decays(),
                             config.get_log_nonsensitive(),
                             config.get_log_nointeraction(),
                             config.get_log_errors());

    if (print_prog_bar) cout << "[" << flush;

    const size_t interactions_soft_max = 100000;
    singles_stream.get_buffer().reserve(interactions_soft_max + 50);
    while (sources.GetTime() < sources.GetEndTime()) {
        while (sources.GetTime() < sources.GetEndTime()) {
            NuclearDecay decay = sources.Decay();
            ray_tracer.TraceDecay(decay, singles_stream.get_buffer());
            if (interactions_soft_max < singles_stream.get_buffer().size()) {
                break;
            }
        }
        singles_stream.process_hits();
        if (config.get_log_hits()) {
            output_hits.LogHits(singles_stream.hits_begin(), singles_stream.hits_end());
        }

        singles_stream.process_singles();
        if (config.get_log_singles() || config.get_log_coinc()) {
            if (config.get_log_singles()) {
                output_singles.LogSingles(singles_stream.singles_begin(),
                                          singles_stream.singles_end());
            }

            for (size_t idx = 0; idx < singles_stream.no_coinc_processes(); idx++) {
                singles_stream.process_coinc(idx);
                if (config.get_log_coinc()) {
                    outputs_coinc[idx].LogCoinc(singles_stream.coinc_begin(),
                                                singles_stream.coinc_end(),
                                                true);
                }
            }
        }

        singles_stream.clear_complete();

        for (; current_tick < (sources.GetElapsedTime() / tick_mark);
             current_tick++)
        {
            if (print_prog_bar) cout << "=" << flush;
        }
    }

    singles_stream.stop_hits();
    if (config.get_log_hits()) {
        output_hits.LogHits(singles_stream.hits_begin(), singles_stream.hits_end());
        output_hits.Close();
    }

    singles_stream.stop_singles();
    if (config.get_log_singles() || config.get_log_coinc()) {
        if (config.get_log_singles()) {
            output_singles.LogSingles(singles_stream.singles_begin(),
                                      singles_stream.singles_end());
            output_singles.Close();
        }

        for (size_t idx = 0; idx < singles_stream.no_coinc_processes(); idx++) {
            singles_stream.stop_coinc(idx);
            if (config.get_log_coinc()) {
                outputs_coinc[idx].LogCoinc(singles_stream.coinc_begin(),
                                            singles_stream.coinc_end(), true);
                outputs_coinc[idx].Close();
            }
        }
    }
    if (print_prog_bar) cout << "=] Done." << endl;
    cout << "\n______________\n Stats\n______________\n"
         << ray_tracer.statistics() << endl;
    if (config.get_log_singles() || config.get_log_coinc()) {
        cout << "______________\n DAQ Stats\n______________\n"
        << singles_stream << endl;
    }
    // A NoOp function if MPI is not enabled.
    Mpi::CombineFiles(config, output_hits, output_singles, outputs_coinc);
    Mpi::Finalize();
}
