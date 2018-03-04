#ifndef SIMULATION_H_
#define SIMULATION_H_

#include <vector>
#include "Gray/Daq/DaqModel.h"
#include "Gray/Gray/SimulationStats.h"
#include "Gray/Output/Output.h"
#include "Gray/Sources/SourceList.h"

class Config;
class SceneDescription;

class Simulation {
public:
    Simulation(
            const Config& config,
            const SceneDescription& scene,
            const SourceList& sources,
            const DaqModel& daq_model,
            size_t thread_idx, size_t no_threads);
    Simulation(Simulation&&) = default;
    SimulationStats Run();
    static void CombineOutputs(
            const Config& config,
            const std::vector<Simulation>& sims);

    Output output_hits;
    Output output_singles;
    std::vector<Output> outputs_coinc;

private:
    SourceList sources;
    DaqModel daq_model;
    size_t thread_idx;
    const SceneDescription& scene;
    const Config& config;

};

#endif // SIMULATION_H_
