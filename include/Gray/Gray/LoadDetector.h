#ifndef LOAD_DETECTOR_H_
#define LOAD_DETECTOR_H_

#include <string>
#include <vector>
#include "Gray/Graphics/ViewableTriangle.h"

class Config;
class CameraView;
class DetectorArray;
class Material;
class RigidMapR3;
class SourceList;
class SceneDescription;
class VectorSource;
class VectorR3;
class AABB;

class LoadDetector {
public:
    static bool Load(const std::string & filename,
                     SceneDescription & theScene,
                     SourceList & sources,
                     Config & config,
                     DetectorArray & detector_array);
    static bool LoadConfig(const std::string & filename, Config & config);
    static std::vector<ViewableTriangle> MakeAnnulusCylinder(
            const VectorR3 & center, const VectorR3 & axis,
            double radius_inner, double radius_outer, double width,
            int det_id, Material * material);
    static std::vector<ViewableTriangle> MakeAnnulusCylinder(
            double radius_inner, double radius_outer, double width);
private:
    static bool ReadVertexR3(VectorR3& vert, const std::string & line);
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
    static bool ProcessFaceDFF(int numVerts,
                               const Material* curMaterial,
                               const std::vector<std::string> & lines,
                               bool parse_VectorSource,
                               int det_id,
                               SceneDescription & scene,
                               double polygonScale,
                               const RigidMapR3 & current_matrix);
    static std::string ScanForSecondField(const std::string & inbuf);
    static bool HandleConfigCommand(const std::string & command,
                                    const std::string & args,
                                    const std::string & file_dir,
                                    Config & config);
    static int IncrementDetector(const RigidMapR3 & current_matrix,
                                 DetectorArray & detector_array);
    static void DisableRayleigh(SceneDescription& scene);
};


#endif /*LOAD_DETECTOR_H_*/
