#ifndef LOAD_DETECTOR_H_
#define LOAD_DETECTOR_H_

#include <stack>
#include <string>
#include <VrMath/LinearR3.h>
#include <Gray/GammaRayTrace.h>
#include <Graphics/SceneDescription.h>

class Material;
class VectorSource;

class LoadDetector
{
public:
    LoadDetector();
    bool Load(const std::string & filename, SceneDescription& theScene, GammaRayTrace &Gray );
private:
    static int GetCommandNumber(const char * cmd);
    static bool ReadVertexR3(VectorR3 & vert, std::ifstream & curFile);
    static void ProcessDetector(const VectorR3 & detCenter,
                                const VectorR3 & detSize,
                                const Material * curMaterial,
                                int id,
                                SceneDescription & scene,
                                const RigidMapR3 & current_matrix);
    static void SetCameraViewInfo(CameraView& theView,
                                  const VectorR3& viewPos,
                                  const VectorR3& lookAtPos,
                                  const VectorR3& upVector, double fovy,
                                  int screenWidth, int screenHeight,
                                  double nearClipping);
    void PushMatrix();
    void PopMatrix();
    static void ApplyTranslation(const VectorR3&t,
                                 RigidMapR3 & current_matrix);
    static void ApplyRotation(const VectorR3& axis,
                              double theta,
                              RigidMapR3 & current_matrix);
    static bool ProcessFaceDFF(int numVerts, const Material* mat,
                               std::ifstream & curFile, VectorSource * s,
                               bool parse_VectorSource, unsigned id,
                               SceneDescription & theScene,
                               double polygonScale,
                               const RigidMapR3 & current_matrix);
    RigidMapR3 &curMatrix();
    std::stack<RigidMapR3*> MatrixStack;
    static std::string ScanForSecondField(const std::string & inbuf);

};


#endif /*LOAD_DETECTOR_H_*/
