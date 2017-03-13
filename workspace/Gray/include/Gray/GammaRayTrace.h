#ifndef GAMMARAYTRACE_H
#define GAMMARAYTRACE_H

#include <Physics/Interaction.h>

class GammaMaterial;
class IntersectKdTree;
class Output;
class Photon;
class SourceList;

class GammaRayTrace {
public:
    static void TraceSources(SourceList & sources,
                             Output & output,
                             IntersectKdTree & tree,
                             GammaMaterial const * const default_material);
private:
    static Interaction::INTER_TYPE GrayTrace(Photon &photon, Output & output,
                                             IntersectKdTree & tree,
                                             GammaMaterial const * const default_material,
                                             GammaMaterial const * const start_material,
                                             int MaxTraceDepth);
    static const double Epsilon;
};

#endif /*GAMMARAYTRACE_H*/
