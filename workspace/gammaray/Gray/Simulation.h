#ifndef SIMULATION_H_
#define SIMULATION_H_

#include <vector>

class Config;
class SourceList;
class SceneDescription;
class Output;
class DaqModel;
class GammaMaterial;

class Simulation {
public:
    static void SetupSeed(const Config & config);
    static void SetupSources(const Config & config, SourceList & sources,
                             SceneDescription & scene);
    static int SetupOutput(const Config & config, Output & output_hits,
                           Output & output_singles,
                           std::vector<Output> & outputs_coinc);

    static void RunSim(const Config & config, SourceList & sources,
                       const SceneDescription & scene,
                       Output & output_hits, Output & output_singles,
                       std::vector<Output> & outputs_coinc,
                       DaqModel & daq_model);
};

#endif // SIMULATION_H_
