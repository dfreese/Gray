#ifndef GAMMARAYTRACE_H
#define GAMMARAYTRACE_H

#include <vector>
#include <ostream>
#include <stack>
#include <Physics/Interaction.h>
#include <Physics/Photon.h>

class GammaMaterial;
class GammaRayTraceStats;
class SceneDescription;
class Output;
class NuclearDecay;
class VectorR3;

class GammaRayTrace {
public:

    GammaRayTrace(const SceneDescription & scene,
                  const std::vector<VectorR3>& source_positions,
                  bool log_nondepositing_inter,
                  bool log_nuclear_decays_inter,
                  bool log_nonsensitive_inter,
                  bool log_nointeractions_inter,
                  bool log_errors_inter);

    std::vector<Interaction> TraceDecay(const NuclearDecay& decay,
            GammaRayTraceStats& stats) const;

    static std::stack<GammaMaterial const *> BuildStack(
            const SceneDescription& scene,
            const VectorR3& src_pos);
    static std::vector<std::stack<GammaMaterial const *>> BuildStacks(
            const SceneDescription& scene,
            const std::vector<VectorR3>& positions);
    static std::stack<GammaMaterial const *> UpdateStack(
            const VectorR3 & src_pos, const VectorR3 & pos,
            const SceneDescription & scene,
            const std::stack<GammaMaterial const *>& base);

private:
    void TracePhoton(Photon photon,
                     std::vector<Interaction> & interactions,
                     std::stack<GammaMaterial const *> MatStack,
                     GammaRayTraceStats& stats) const;
    std::stack<GammaMaterial const *> DecayStack(
            size_t src_id, const VectorR3 & pos) const;
    const GammaMaterial& SourceMaterial(size_t idx) const;


    const SceneDescription & scene;
    const std::vector<VectorR3> source_positions;
    const std::vector<std::stack<GammaMaterial const *>> source_mats;
    const bool log_nondepositing_inter;
    const bool log_nuclear_decays;
    const bool log_nonsensitive;
    const bool log_nointeractions;
    const bool log_errors;
    const int max_trace_depth;
};

#endif /*GAMMARAYTRACE_H*/
