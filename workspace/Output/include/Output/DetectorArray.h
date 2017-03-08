#ifndef DETECTORARRAY_H_
#define DETECTORARRAY_H_
#include <Output/Detector.h>
#include <vector>

class VectorR3;
class RigidMapR3;

class DetectorArray
{
public:
    DetectorArray();
    ~DetectorArray();

    // returns detector_id
    unsigned AddDetector(const VectorR3 & pos, const VectorR3 &size,
                         const RigidMapR3 & map,unsigned x, unsigned y,
                         unsigned z, unsigned bl);
    void OutputDetectorArray();
    friend ostream& operator<< ( ostream& os, const DetectorArray& d );
public:
    std::vector<Detector*> d;
};

#endif /*DETECTORARRAY_H_*/
