#ifndef GAMMARAYTRACE_H
#define GAMMARAYTRACE_H

#include <stack>
#include <string>
#include <Sources/SourceList.h>
#include <Output/Output.h>
#include <Physics/Interaction.h>

class GammaMaterial;
class Photon;
class IntersectKdTree;

class GammaRayTrace
{
public:
    GammaRayTrace();
    void GRayTraceSources(SourceList & sources, Output & output);
    void SetDefaultMaterial(GammaMaterial * mat);
    void SetKdTree(IntersectKdTree & tree);
private:
    Interaction::INTER_TYPE GRayTrace(Photon &photon, Output & output,
                                      GammaMaterial const * const start_material,
                                      int MaxTraceDepth);
    GammaMaterial * defaultMat;
    const double Epsilon = 1e-10;
    IntersectKdTree * kd_tree;
};

#endif /*GAMMARAYTRACE_H*/
