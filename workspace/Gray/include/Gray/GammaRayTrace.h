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
    ~GammaRayTrace();
    void GRayTraceSources(void);
    void AddSource(Source * s);
    void SetDefaultMaterial(GammaMaterial * mat);
    void SetFileNameOutput(const std::string & name);
    void SetSimulationTime(double time);
    Output output;
    SourceList sources;
    void SetKdTree(IntersectKdTree & tree);
private:
    Interaction::INTER_TYPE GRayTrace(Photon &photon,
                                      GammaMaterial const * const start_material,
                                      int MaxTraceDepth);
    GammaMaterial * defaultMat;
    double simulationTime;
    const double Epsilon = 1e-10;
    IntersectKdTree * kd_tree;
};

#endif /*GAMMARAYTRACE_H*/
