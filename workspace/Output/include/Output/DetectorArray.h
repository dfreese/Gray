#ifndef DETECTORARRAY_H_
#define DETECTORARRAY_H_
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
    void OutputDetectorArray();
    friend std::ostream& operator<< (std::ostream& os, const DetectorArray& d );
    std::vector<Detector> detectors;
};

#endif /*DETECTORARRAY_H_*/
