#ifndef LOAD_DETECTOR_H_
#define LOAD_DETECTOR_H_

#include <stack>
#include <string>
#include <VrMath/LinearR3.h>
#include <Gray/GammaRayTrace.h>
#include <Graphics/SceneDescription.h>

class Material;
class VectorSource;
#define MAX_INCLUDE 256

class LoadDetector
{
public:
    LoadDetector();
    bool Load(const std::string & filename, SceneDescription& theScene, GammaRayTrace &Gray );
private:
    SceneDescription* ScenePtr;
    static int GetCommandNumber(const char * cmd);
    static bool ReadVertexR3(VectorR3 & vert, std::ifstream & curFile);
    void ProcessDetector( const VectorR3& detCenter, const VectorR3& detSize, const Material* curMaterial, int id );
    void SetCameraViewInfo( CameraView& theView,
                            const VectorR3& viewPos, const VectorR3& lookAtPos,
                            const VectorR3& upVector, double fovy,
                            int screenWidth, int screenHeight, double nearClipping );
    void PushMatrix();
    void PopMatrix();
    void ApplyTranslation(const VectorR3&t);
    void ApplyRotation(const VectorR3& axis, double theta);
    static bool ProcessFaceDFF(int numVerts, const Material* mat,
                               std::ifstream & curFile, VectorSource * s,
                               bool parse_VectorSource, unsigned id,
                               SceneDescription & theScene,
                               double polygonScale,
                               const RigidMapR3 & current_matrix);
    RigidMapR3 &curMatrix();
    std::stack<RigidMapR3*> MatrixStack;
    double polygonScale;
    double actScale;

    double time_resolution;
    double energy_resolution;

    double time_gate;
    double energy_gate_lower;
    double energy_gate_upper;

    bool positronRange;
    bool positronRangeCusp;
    double positronC;
    double positronK1;
    double positronK2;
    double positronMaxRange;
    unsigned int block_id;

    static std::string ScanForSecondField(const std::string & inbuf);

};


#endif /*LOAD_DETECTOR_H_*/
