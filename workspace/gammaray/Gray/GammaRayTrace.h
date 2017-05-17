#ifndef GAMMARAYTRACE_H
#define GAMMARAYTRACE_H

#include <vector>
#include <ostream>

class GammaMaterial;
class Interaction;
class IntersectKdTree;
class Output;
class Photon;
class SourceList;

class GammaRayTrace {
public:
    struct TraceStats {
        long events = 0;
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

    static void TraceSources(SourceList & sources,
                             const IntersectKdTree & tree,
                             std::vector<Interaction> & interactions,
                             size_t soft_max_interactions,
                             GammaMaterial const * const default_material,
                             bool log_nuclear_decays,
                             bool log_nonsensitive,
                             bool log_nointeractions,
                             bool log_errors,
                             TraceStats & stats);
private:
    static void TracePhoton(Photon &photon,
                            std::vector<Interaction> & interactions,
                            const IntersectKdTree & tree,
                            GammaMaterial const * const default_material,
                            GammaMaterial const * const start_material,
                            int MaxTraceDepth,
                            bool log_nonsensitive,
                            bool log_nointeractions,
                            bool log_errors,
                            TraceStats & stats);
    static const double Epsilon;
};

#endif /*GAMMARAYTRACE_H*/
