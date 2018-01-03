#ifndef DETECTORARRAY_H_
#define DETECTORARRAY_H_
#include <map>
#include <vector>
#include <ostream>
#include <Output/Detector.h>

class VectorR3;
class RigidMapR3;

class DetectorArray
{
public:
    DetectorArray();
    // returns detector_id
    int AddDetector(const VectorR3 & pos, const VectorR3 &size,
                    const RigidMapR3 & map, int x, int y, int z, int bl);
    std::map<std::string, std::vector<int>> default_mapping();
    void OutputDetectorArray();
    bool WritePositions(std::ostream& os) const;
    bool WritePositions(const std::string& filename) const;
    std::vector<Detector> detectors;
    void WriteBasicMap(std::ostream & os,
                       const std::string & detector_name,
                       const std::string & block_name,
                       const std::string & bx_name,
                       const std::string & by_name,
                       const std::string & bz_name);
};

#endif /*DETECTORARRAY_H_*/
