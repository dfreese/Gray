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
    static void TraceSources(SourceList & sources,
                             Output & output,
                             IntersectKdTree & tree,
                             int num_decays,
                             GammaMaterial const * const default_material);
private:
    static void TracePhoton(Photon &photon,
                            std::vector<Interaction> & interactions,
                            IntersectKdTree & tree,
                            GammaMaterial const * const default_material,
                            GammaMaterial const * const start_material,
                            int MaxTraceDepth);
    static const double Epsilon;
};

#endif /*GAMMARAYTRACE_H*/
