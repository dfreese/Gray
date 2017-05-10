#ifndef LOAD_DETECTOR_H_
#define LOAD_DETECTOR_H_

#include <string>

class Config;
class CameraView;
class DetectorArray;
class Material;
class RigidMapR3;
class SourceList;
class SceneDescription;
class VectorSource;
class VectorR3;

class LoadDetector {
public:
    static bool Load(const std::string & filename,
                     SceneDescription & theScene,
                     SourceList & sources,
                     Config & config,
                     DetectorArray & detector_array);
private:
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
    static void ApplyTranslation(const VectorR3&t,
                                 RigidMapR3 & current_matrix);
    static void ApplyRotation(const VectorR3& axis,
                              double theta,
                              RigidMapR3 & current_matrix);
    static bool ProcessFaceDFF(int numVerts, const Material* mat,
                               std::ifstream & curFile, VectorSource * s,
                               bool parse_VectorSource, int det_id,
                               SceneDescription & theScene,
                               double polygonScale,
                               const RigidMapR3 & current_matrix);
    static std::string ScanForSecondField(const std::string & inbuf);

};


#endif /*LOAD_DETECTOR_H_*/
