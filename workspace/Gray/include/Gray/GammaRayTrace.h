#ifndef GAMMARAYTRACE_H
#define GAMMARAYTRACE_H

#include <stack>
#include <string>
#include <Sources/SourceList.h>
#include <Output/Output.h>

class GammaMaterial;
class Photon;

extern long SeekIntersectionKd(const VectorR3& startPos, const VectorR3& direction,
                               double *hitDist, VisiblePoint& returnedPoint,
                               long avoidK = -1);

const double Epsilon = 1e-10;
const double s1_SOL = (1.0 / 29979245800.0);

class GammaRayTrace
{
public:
    GammaRayTrace();
    ~GammaRayTrace();
    void GRayTraceSources(void);
    void AddSource(Source & s);
    void SetDefaultMaterial(GammaMaterial * mat);
    void SetFileNameOutput(const std::string & name);
    void SetSimulationTime(double time);
    Output output;
    SourceList sources;
private:
    INTER_TYPE GRayTrace(VisiblePoint &visPoint,
                         int TraceDepth, Photon &photon,
                         std::stack<GammaMaterial const * const> & MatStack, long avoidK);
    GammaMaterial * defaultMat;
    double simulationTime;

};

#endif /*GAMMARAYTRACE_H*/
