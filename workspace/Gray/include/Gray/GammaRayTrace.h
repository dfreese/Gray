#ifndef GAMMARAYTRACE_H
#define GAMMARAYTRACE_H

#include <vector>

class GammaMaterial;
class Interaction;
class IntersectKdTree;
class Output;
class Photon;
class SourceList;

class GammaRayTrace {
public:
    struct TraceStats {
        long count_decays = 0;
        long count_photons = 0;
        long count_photoelectric = 0;
        long count_xray_escape = 0;
        long count_compton = 0;
        long count_rayleigh = 0;
        long count_error = 0;
    };

    static long TraceSources(SourceList & sources,
                             IntersectKdTree & tree,
                             std::vector<Interaction> & interactions,
                             size_t soft_max_interactions,
                             GammaMaterial const * const default_material,
                             bool log_nuclear_decays,
                             bool log_nonsensitive,
                             bool log_errors);
private:
    static void TracePhoton(Photon &photon,
                            std::vector<Interaction> & interactions,
                            IntersectKdTree & tree,
                            GammaMaterial const * const default_material,
                            GammaMaterial const * const start_material,
                            int MaxTraceDepth,
                            bool log_nonsensitive,
                            bool log_errors);
    static const double Epsilon;
};

#endif /*GAMMARAYTRACE_H*/
