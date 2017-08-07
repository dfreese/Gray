#ifndef SIMULATION_H_
#define SIMULATION_H_

#include <vector>

class Config;
class SourceList;
class IntersectKdTree;
class Output;
class InteractionStream;
class GammaMaterial;

class Simulation {
public:
    static void SetupSeed(const Config & config);
    static void SetupSources(const Config & config, SourceList & sources,
                             IntersectKdTree & intersect_kd_tree,
                             GammaMaterial const * const default_material);
    static int SetupOutput(const Config & config, Output & output_hits,
                           Output & output_singles,
                           std::vector<Output> & outputs_coinc);

    static void RunSim(const Config & config, SourceList & sources,
                       const IntersectKdTree & intersect_kd_tree,
                       Output & output_hits, Output & output_singles,
                       std::vector<Output> & outputs_coinc,
                       InteractionStream & singles_stream,
                       GammaMaterial* default_material);
};

#endif // SIMULATION_H_
