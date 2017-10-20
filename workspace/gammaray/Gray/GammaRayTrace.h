#ifndef GAMMARAYTRACE_H
#define GAMMARAYTRACE_H

#include <vector>
#include <ostream>
#include <stack>

class GammaMaterial;
class Interaction;
class SceneDescription;
class Output;
class Photon;
class SourceList;
class VectorR3;

class GammaRayTrace {
public:

    GammaRayTrace(SourceList & source_list,
                  const SceneDescription & scene,
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
    void TraceSources(std::vector<Interaction> & interactions,
                      size_t soft_max_interactions);

private:
    void TracePhoton(Photon &photon,
                     std::vector<Interaction> & interactions,
                     std::stack<GammaMaterial const *> MatStack);

    SourceList & sources;
    const SceneDescription & scene;
    bool log_nuclear_decays;
    bool log_nonsensitive;
    bool log_nointeractions;
    bool log_errors;
    TraceStats stats;
    int max_trace_depth;
};

#endif /*GAMMARAYTRACE_H*/
