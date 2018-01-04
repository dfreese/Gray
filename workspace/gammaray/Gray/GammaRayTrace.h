#ifndef GAMMARAYTRACE_H
#define GAMMARAYTRACE_H

#include <vector>
#include <ostream>
#include <stack>
#include <Physics/NuclearDecay.h>

class GammaMaterial;
class Interaction;
class SceneDescription;
class Output;
class Photon;
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

    struct TraceStats {
        long decays = 0;
        long photons = 0;
        long no_interaction = 0;
        long photoelectric = 0;
        long xray_escape = 0;
        long compton = 0;
        long rayleigh = 0;
        long photoelectric_sensitive = 0;
        long xray_escape_sensitive = 0;
        long compton_sensitive = 0;
        long rayleigh_sensitive = 0;
        long error = 0;
        friend std::ostream & operator<< (std::ostream & os,
                                          const TraceStats& s);
    };


    const TraceStats & statistics() const;
    void TraceDecay(NuclearDecay & decay,
                    std::vector<Interaction> & interactions) const;
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
    void TracePhoton(Photon &photon,
                     std::vector<Interaction> & interactions,
                     std::stack<GammaMaterial const *> MatStack) const;
    std::stack<GammaMaterial const *> DecayStack(
            size_t src_id, const VectorR3 & pos) const;
    const GammaMaterial& SourceMaterial(size_t idx) const;


    const SceneDescription & scene;
    const std::vector<VectorR3> source_positions;
    const std::vector<std::stack<GammaMaterial const *>> source_mats;
    bool log_nondepositing_inter;
    bool log_nuclear_decays;
    bool log_nonsensitive;
    bool log_nointeractions;
    bool log_errors;
    mutable TraceStats stats;
    int max_trace_depth;
};

#endif /*GAMMARAYTRACE_H*/
